//TODO: copyrights
#include "commandqueue.h"
#include "context.h"
#include "device_interface.h"
#include "propertylist.h"
#include "events.h"

#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <Windows.h>

using namespace Devices;

/*
* CommandQueue
*/

CommandQueue::CommandQueue(Context *ctx,
	DeviceInterface *device,
	cl_command_queue_properties properties,
	cl_int *errcode_ret)
	: Object(Object::T_CommandQueue, ctx), p_device(device),
	p_properties(properties), p_flushed(true)
{
	// Initialize the locking machinery
	p_event_list_mutex = new sw::Resource(0);
	p_event_list_cond = new sw::Event();

	// Check that the device belongs to the context
	if(!ctx->hasDevice(device))
	{
		*errcode_ret = CL_INVALID_DEVICE;
		return;
	}

	*errcode_ret = checkProperties();
}

CommandQueue::~CommandQueue()
{
	// Free the mutex
	p_event_list_mutex->lock(sw::DESTRUCT);
	p_event_list_mutex->unlock();
	p_event_list_mutex->destruct();
	
	p_event_list_cond->signal();
	delete p_event_list_cond;
}

cl_int CommandQueue::info(cl_command_queue_info param_name,
	size_t param_value_size,
	void *param_value,
	size_t *param_value_size_ret) const
{
	void *value = 0;
	size_t value_length = 0;

	union {
		cl_uint cl_uint_var;
		cl_device_id cl_device_id_var;
		cl_context cl_context_var;
		cl_command_queue_properties cl_command_queue_properties_var;
	};

	switch(param_name)
	{
	case CL_QUEUE_CONTEXT:
		SIMPLE_ASSIGN(cl_context, parent());
		break;

	case CL_QUEUE_DEVICE:
		SIMPLE_ASSIGN(cl_device_id, p_device);
		break;

	case CL_QUEUE_REFERENCE_COUNT:
		SIMPLE_ASSIGN(cl_uint, references());
		break;

	case CL_QUEUE_PROPERTIES:
		SIMPLE_ASSIGN(cl_command_queue_properties, p_properties);
		break;

	default:
		return CL_INVALID_VALUE;
	}

	if(param_value && param_value_size < value_length)
		return CL_INVALID_VALUE;

	if(param_value_size_ret)
		*param_value_size_ret = value_length;

	if(param_value)
		std::memcpy(param_value, value, value_length);

	return CL_SUCCESS;
}

cl_int CommandQueue::setProperty(cl_command_queue_properties properties,
	cl_bool enable,
	cl_command_queue_properties *old_properties)
{
	if(old_properties)
		*old_properties = p_properties;

	if(enable)
		p_properties |= properties;
	else
		p_properties &= ~properties;

	return checkProperties();
}

cl_int CommandQueue::checkProperties() const
{
	// Check that all the properties are valid
	cl_command_queue_properties properties =
		CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE |
		CL_QUEUE_PROFILING_ENABLE;

	if((p_properties & properties) != p_properties)
		return CL_INVALID_VALUE;

	// Check that the device handles these properties
	cl_int result;

	result = p_device->info(CL_DEVICE_QUEUE_PROPERTIES,
		sizeof(cl_command_queue_properties),
		&properties,
		0);

	if(result != CL_SUCCESS)
		return result;

	if((p_properties & properties) != p_properties)
		return CL_INVALID_QUEUE_PROPERTIES;

	return CL_SUCCESS;
}

void CommandQueue::flush()
{
	// Wait for the command queue to be in state "flushed".
	p_event_list_mutex->lock(sw::PRIVATE);

	while(!p_flushed)
		p_event_list_cond->wait();

	p_event_list_mutex->unlock();
}

void CommandQueue::finish()
{
	// As pushEventsOnDevice doesn't remove SUCCESS events, we may need
	// to do that here in order not to be stuck.
	cleanEvents();

	// All the queued events must have completed. When they are, they get
	// deleted from the command queue, so simply wait for it to become empty.
	p_event_list_mutex->lock(sw::PRIVATE);

	while(p_events.size() != 0)
		p_event_list_cond->wait();

	p_event_list_mutex->unlock();
}

cl_int CommandQueue::queueEvent(Event *event)
{
	// Let the device initialize the event (for instance, a pointer at which
	// memory would be mapped)
	cl_int rs = p_device->initEventDeviceData(event);

	if(rs != CL_SUCCESS)
		return rs;

	// Append the event at the end of the list
	p_event_list_mutex->lock(sw::PRIVATE);

	p_events.push_back(event);
	p_flushed = false;

	p_event_list_mutex->unlock();

	// Timing info if needed
	if(p_properties & CL_QUEUE_PROFILING_ENABLE)
		event->updateTiming(Event::Queue);

	// Explore the list for events we can push on the device
	pushEventsOnDevice();

	return CL_SUCCESS;
}

void CommandQueue::cleanEvents()
{
	p_event_list_mutex->lock(sw::PRIVATE);

	std::list<Event *>::iterator it = p_events.begin(), oldit;

	while(it != p_events.end())
	{
		Event *event = *it;

		if(event->status() == Event::Complete)
		{
			// We cannot be deleted from inside us
			event->setReleaseParent(false);
			oldit = it;
			++it;

			p_events.erase(oldit);
			clReleaseEvent((cl_event)event);
		}
		else
		{
			++it;
		}
	}

	// We have cleared the list, so wake up the sleeping threads
	if(p_events.size() == 0)
		p_event_list_cond->signal();

	p_event_list_mutex->unlock();

	// Check now if we have to be deleted
	if(references() == 0)
		delete this;
}

void CommandQueue::pushEventsOnDevice()
{
	p_event_list_mutex->lock(sw::PRIVATE);
	// Explore the events in p_events and push on the device all of them that
	// are :
	//
	// - Not already pushed (in Event::Queued state)
	// - Not after a barrier, except if we begin with a barrier
	// - If we are in-order, only the first event in Event::Queued state can
	//   be pushed

	std::list<Event *>::iterator it = p_events.begin();
	bool first = true;

	// We assume that we will flush the command queue (submit all the events)
	// This will be changed in the while() when we know that not all events
	// are submitted.
	p_flushed = true;

	while(it != p_events.end())
	{
		Event *event = *it;

		// If the event is completed, remove it
		if(event->status() == Event::Complete)
		{
			++it;
			continue;
		}

		// We cannot do out-of-order, so we can only push the first event.
		if((p_properties & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE) == 0 &&
			!first)
		{
			p_flushed = false; // There are remaining events.
			break;
		}

		// Stop if we encounter a barrier that isn't the first event in the list.
		if(event->type() == Event::Barrier && !first)
		{
			// We have events to wait, stop
			p_flushed = false;
			break;
		}

		// Completed events and first barriers are out, it remains real events
		// that have to block in-order execution.
		first = false;

		// If the event is not "pushable" (in Event::Queued state), skip it
		// It is either Submitted or Running.
		if(event->status() != Event::Queued)
		{
			++it;
			continue;
		}

		// Check that all the waiting-on events of this event are finished
		cl_uint count;
		const Event **event_wait_list;
		bool skip_event = false;

		event_wait_list = event->waitEvents(count);

		for(cl_uint i = 0; i<count; ++i)
		{
			if(event_wait_list[i]->status() != Event::Complete)
			{
				skip_event = true;
				p_flushed = false;
				break;
			}
		}

		if(skip_event)
		{
			// If we encounter a WaitForEvents event that is not "finished",
			// don't push events after it.
			if(event->type() == Event::WaitForEvents)
				break;

			// The event has its dependencies not already met.
			++it;
			continue;
		}

		// The event can be pushed, if we need to
		if(!event->isDummy())
		{
			if(p_properties & CL_QUEUE_PROFILING_ENABLE)
				event->updateTiming(Event::Submit);

			event->setStatus(Event::Submitted);
			p_device->pushEvent(event);
		}
		else
		{
			// Set the event as completed. This will call pushEventsOnDevice,
			// again, so release the lock to avoid a deadlock. We also return
			// because the recursive call will continue our work.
			p_event_list_mutex->unlock();
			event->setStatus(Event::Complete);
			return;
		}
	}

	if(p_flushed)
		p_event_list_cond->signal();

	p_event_list_mutex->unlock();
}

Event **CommandQueue::events(unsigned int &count)
{
	Event **result;

	p_event_list_mutex->lock(sw::PRIVATE);

	count = p_events.size();
	result = (Event **)std::malloc(count * sizeof(Event *));

	// Copy each event of the list into result, retaining them
	unsigned int index = 0;
	std::list<Event *>::iterator it = p_events.begin();

	while(it != p_events.end())
	{
		result[index] = *it;
		result[index]->reference();

		++it;
		++index;
	}

	// Now result contains an immutable list of events. Even if the events
	// become completed in another thread while result is used, the events
	// are retained and so guaranteed to remain valid.
	p_event_list_mutex->unlock();

	return result;
}

/*
* Event
*/

Event::Event(CommandQueue *parent,
	Status status,
	cl_uint num_events_in_wait_list,
	const Event **event_wait_list,
	cl_int *errcode_ret)
	: Object(Object::T_Event, parent),
	p_num_events_in_wait_list(num_events_in_wait_list), p_event_wait_list(0),
	p_status(status), p_device_data(0)
{
	// Initialize the locking machinery
	p_state_change_cond = new sw::Event();
	p_state_mutex = new sw::Resource(0);

	std::memset(&p_timing, 0, sizeof(p_timing));

	// Check sanity of parameters
	if(!event_wait_list && num_events_in_wait_list)
	{
		*errcode_ret = CL_INVALID_EVENT_WAIT_LIST;
		return;
	}

	if(event_wait_list && !num_events_in_wait_list)
	{
		*errcode_ret = CL_INVALID_EVENT_WAIT_LIST;
		return;
	}

	// Check that none of the events in event_wait_list is in an error state
	for(cl_uint i = 0; i<num_events_in_wait_list; ++i)
	{
		if(event_wait_list[i] == 0)
		{
			*errcode_ret = CL_INVALID_EVENT_WAIT_LIST;
			return;
		}
		else if(event_wait_list[i]->status() < 0)
		{
			*errcode_ret = CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
			return;
		}
	}

	// Allocate a new buffer for the events to wait
	if(num_events_in_wait_list)
	{
		const unsigned int len = num_events_in_wait_list * sizeof(Event *);
		p_event_wait_list = (const Event **)std::malloc(len);

		if(!p_event_wait_list)
		{
			*errcode_ret = CL_OUT_OF_HOST_MEMORY;
			return;
		}

		std::memcpy((void *)p_event_wait_list, (void *)event_wait_list, len);
	}

	// Explore the events we are waiting on and reference them
	for(cl_uint i = 0; i<num_events_in_wait_list; ++i)
	{
		clRetainEvent((cl_event)event_wait_list[i]);

		if(event_wait_list[i]->type() == Event::User && parent)
			((UserEvent *)event_wait_list[i])->addDependentCommandQueue(parent);
	}
}

void Event::freeDeviceData()
{
	if(parent() && p_device_data)
	{
		DeviceInterface *device = 0;
		((CommandQueue *)parent())->info(CL_QUEUE_DEVICE, sizeof(DeviceInterface *), &device, 0);

		device->freeEventDeviceData(this);
	}
}

Event::~Event()
{
	for(cl_uint i = 0; i<p_num_events_in_wait_list; ++i)
		clReleaseEvent((cl_event)p_event_wait_list[i]);

	if(p_event_wait_list)
		std::free((void *)p_event_wait_list);

	// Free the mutex
	p_state_mutex->lock(sw::DESTRUCT);
	p_state_mutex->unlock();
	p_state_mutex->destruct();

	p_state_change_cond->signal();
	delete p_state_change_cond;
}

bool Event::isDummy() const
{
	// A dummy event has nothing to do on an execution device and must be
	// completed directly after being "submitted".

	switch(type())
	{
	case Marker:
	case User:
	case Barrier:
	case WaitForEvents:
		return true;

	default:
		return false;
	}
}

void Event::setStatus(Status status)
{
	// TODO: If status < 0, terminate all the events depending on us.
	p_state_mutex->lock(sw::PRIVATE);
	p_status = status;

	p_state_change_cond->signal();

	// Call the callbacks
	std::multimap<Status, CallbackData>::const_iterator it;
	std::pair<std::multimap<Status, CallbackData>::const_iterator,
		std::multimap<Status, CallbackData>::const_iterator> ret;

	ret = p_callbacks.equal_range(status > 0 ? status : Complete);

	for(it = ret.first; it != ret.second; ++it)
	{
		const CallbackData &data = (*it).second;
		data.callback((cl_event)this, p_status, data.user_data);
	}

	p_state_mutex->unlock();

	// If the event is completed, inform our parent so it can push other events
	// to the device.
	if(parent() && status == Complete)
		((CommandQueue *)parent())->pushEventsOnDevice();
	else if(type() == Event::User)
		((UserEvent *)this)->flushQueues();
}

void Event::setDeviceData(void *data)
{
	p_device_data = data;
}

LARGE_INTEGER
getFILETIMEoffset()
{
	SYSTEMTIME s;
	FILETIME f;
	LARGE_INTEGER t;

	s.wYear = 1970;
	s.wMonth = 1;
	s.wDay = 1;
	s.wHour = 0;
	s.wMinute = 0;
	s.wSecond = 0;
	s.wMilliseconds = 0;
	SystemTimeToFileTime(&s, &f);
	t.QuadPart = f.dwHighDateTime;
	t.QuadPart <<= 32;
	t.QuadPart |= f.dwLowDateTime;
	return (t);
}

int
clock_gettime(int X, struct timeval *tv)
{
	LARGE_INTEGER           t;
	FILETIME            f;
	double                  microseconds;
	static LARGE_INTEGER    offset;
	static double           frequencyToMicroseconds;
	static int              initialized = 0;
	static BOOL             usePerformanceCounter = 0;

	if(!initialized) {
		LARGE_INTEGER performanceFrequency;
		initialized = 1;
		usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
		if(usePerformanceCounter) {
			QueryPerformanceCounter(&offset);
			frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
		}
		else {
			offset = getFILETIMEoffset();
			frequencyToMicroseconds = 10.;
		}
	}
	if(usePerformanceCounter) QueryPerformanceCounter(&t);
	else {
		GetSystemTimeAsFileTime(&f);
		t.QuadPart = f.dwHighDateTime;
		t.QuadPart <<= 32;
		t.QuadPart |= f.dwLowDateTime;
	}

	t.QuadPart -= offset.QuadPart;
	microseconds = (double)t.QuadPart / frequencyToMicroseconds;
	t.QuadPart = microseconds;
	tv->tv_sec = t.QuadPart / 1000000;
	tv->tv_usec = t.QuadPart % 1000000;
	return (0);
}

void Event::updateTiming(Timing timing)
{
	if(timing >= Max)
		return;

	p_state_mutex->lock(sw::PRIVATE);

	// Don't update more than one time (NDRangeKernel for example)
	if(p_timing[timing])
	{
		p_state_mutex->unlock();
		return;
	}

	struct timeval tp;
	//struct timespec tp;
	cl_ulong rs;

	//TODO
	if(clock_gettime(0, &tp) != 0)
		clock_gettime(0, &tp);

	rs = tp.tv_usec;// *1000;
	rs += tp.tv_sec * 1000000;
	/*rs = tp.tv_nsec;
	rs += tp.tv_sec * 1000000;*/

	p_timing[timing] = rs;

	p_state_mutex->unlock();
}

Event::Status Event::status() const
{
	// HACK : We need const qualifier but we also need to lock a mutex
	Event *me = (Event *)(void *)this;

	me->p_state_mutex->lock(sw::PRIVATE);

	Status ret = p_status;

	me->p_state_mutex->unlock();

	return ret;
}

void Event::waitForStatus(Status status)
{
	p_state_mutex->lock(sw::PRIVATE);

	while(p_status != status && p_status > 0)
	{
		p_state_change_cond->signal();
	}

	p_state_mutex->unlock();
}

void *Event::deviceData()
{
	return p_device_data;
}

const Event **Event::waitEvents(cl_uint &count) const
{
	count = p_num_events_in_wait_list;
	return p_event_wait_list;
}

void Event::setCallback(cl_int command_exec_callback_type,
	event_callback callback,
	void *user_data)
{
	CallbackData data;

	data.callback = callback;
	data.user_data = user_data;

	p_state_mutex->lock(sw::PRIVATE);

	p_callbacks.insert(std::pair<Status, CallbackData>(
		(Status)command_exec_callback_type,
		data));

	p_state_mutex->unlock();
}

cl_int Event::info(cl_event_info param_name,
	size_t param_value_size,
	void *param_value,
	size_t *param_value_size_ret) const
{
	void *value = 0;
	size_t value_length = 0;

	union {
		cl_command_queue cl_command_queue_var;
		cl_context cl_context_var;
		cl_command_type cl_command_type_var;
		cl_int cl_int_var;
		cl_uint cl_uint_var;
	};

	switch(param_name)
	{
	case CL_EVENT_COMMAND_QUEUE:
		SIMPLE_ASSIGN(cl_command_queue, parent());
		break;

	case CL_EVENT_CONTEXT:
		if(parent())
		{
			SIMPLE_ASSIGN(cl_context, parent()->parent());
		}
		else
		{
			if(type() == User)
				SIMPLE_ASSIGN(cl_context, ((UserEvent *)this)->context())
			else
			SIMPLE_ASSIGN(cl_context, 0);
		}

	case CL_EVENT_COMMAND_TYPE:
		SIMPLE_ASSIGN(cl_command_type, type());
		break;

	case CL_EVENT_COMMAND_EXECUTION_STATUS:
		SIMPLE_ASSIGN(cl_int, status());
		break;

	case CL_EVENT_REFERENCE_COUNT:
		SIMPLE_ASSIGN(cl_uint, references());
		break;

	default:
		return CL_INVALID_VALUE;
	}

	if(param_value && param_value_size < value_length)
		return CL_INVALID_VALUE;

	if(param_value_size_ret)
		*param_value_size_ret = value_length;

	if(param_value)
		std::memcpy(param_value, value, value_length);

	return CL_SUCCESS;
}

cl_int Event::profilingInfo(cl_profiling_info param_name,
	size_t param_value_size,
	void *param_value,
	size_t *param_value_size_ret) const
{
	if(type() == Event::User)
		return CL_PROFILING_INFO_NOT_AVAILABLE;

	// Check that the Command Queue has profiling enabled
	cl_command_queue_properties queue_props;
	cl_int rs;

	rs = ((CommandQueue *)parent())->info(CL_QUEUE_PROPERTIES,
		sizeof(cl_command_queue_properties),
		&queue_props, 0);

	if(rs != CL_SUCCESS)
		return rs;

	if((queue_props & CL_QUEUE_PROFILING_ENABLE) == 0)
		return CL_PROFILING_INFO_NOT_AVAILABLE;

	//TODO: fix this
	/*if(status() != Event::Complete)
		return CL_PROFILING_INFO_NOT_AVAILABLE;*/

	void *value = 0;
	size_t value_length = 0;
	cl_ulong cl_ulong_var;

	switch(param_name)
	{
	case CL_PROFILING_COMMAND_QUEUED:
		SIMPLE_ASSIGN(cl_ulong, p_timing[Queue]);
		break;

	case CL_PROFILING_COMMAND_SUBMIT:
		SIMPLE_ASSIGN(cl_ulong, p_timing[Submit]);
		break;

	case CL_PROFILING_COMMAND_START:
		SIMPLE_ASSIGN(cl_ulong, p_timing[Start]);
		break;

	case CL_PROFILING_COMMAND_END:
		SIMPLE_ASSIGN(cl_ulong, p_timing[End]);
		break;

	default:
		return CL_INVALID_VALUE;
	}

	if(param_value && param_value_size < value_length)
		return CL_INVALID_VALUE;

	if(param_value_size_ret)
		*param_value_size_ret = value_length;

	if(param_value)
		std::memcpy(param_value, value, value_length);

	return CL_SUCCESS;
}
