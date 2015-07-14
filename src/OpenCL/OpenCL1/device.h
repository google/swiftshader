//TODO: copyrights


#ifndef __CPU_DEVICE_H__
#define __CPU_DEVICE_H__

#define MAX_THREAD_AMOUNT 16

#include <list>

#include "opencl.h"
#include "device_interface.h"
#include "Resource.hpp"

namespace Devices
{

class CPUDevice : public DeviceInterface
{
public:
	CPUDevice();
	~CPUDevice();

	/**
	* \brief Initialize the CPU device
	*
	* This function creates the worker threads and get information about
	* the host system for the \c numCPUs() and \c cpuMhz functions.
	*/
	void init();

	cl_int info(cl_device_info param_name,
		size_t param_value_size,
		void *param_value,
		size_t *param_value_size_ret) const;

	DeviceBuffer *createDeviceBuffer(MemObject *buffer, cl_int *rs);
	DeviceProgram *createDeviceProgram(Program *program);
	DeviceKernel *createDeviceKernel(Kernel *kernel,
		llvm::Function *function);

	cl_int initEventDeviceData(Event *event);
	void freeEventDeviceData(Event *event);

	void pushEvent(Event *event);
	Event *getEvent(bool &stop);

	float cpuMhz() const;           /*!< \brief Speed of the CPU in Mhz */

private:
	unsigned int p_num_events;
	float p_cpu_mhz;
	sw::Thread *p_workers[MAX_THREAD_AMOUNT];
	sw::Resource *eventListResource;
	sw::Event *p_events_cond;
	std::list<Event *> p_events;
	bool p_stop;
};

//class GPUDevice : public DeviceInterface
//{
//public:
//	GPUDevice();
//	~GPUDevice();
//
//	/**
//	* \brief Initialize the CPU device
//	*
//	* This function creates the worker threads and get information about
//	* the host system for the \c numCPUs() and \c cpuMhz functions.
//	*/
//	void init();
//
//	cl_int info(cl_device_info param_name,
//		size_t param_value_size,
//		void *param_value,
//		size_t *param_value_size_ret) const;
//
//	DeviceBuffer *createDeviceBuffer(MemObject *buffer, cl_int *rs);
//	DeviceProgram *createDeviceProgram(Program *program);
//	DeviceKernel *createDeviceKernel(Kernel *kernel,
//		llvm::Function *function);
//
//	cl_int initEventDeviceData(Event *event);
//	void freeEventDeviceData(Event *event);
//
//	void pushEvent(Event *event);
//	Event *getEvent(bool &stop);
//
//	unsigned int numCPUs() const;   /*!< \brief Number of logical CPU cores on the system */
//	float cpuMhz() const;           /*!< \brief Speed of the CPU in Mhz */
//
//private:
//	unsigned int p_cores, p_num_events;
//	float p_cpu_mhz;
//	pthread_t *p_workers;
//
//	std::list<Event *> p_events;
//	pthread_cond_t p_events_cond;
//	pthread_mutex_t p_events_mutex;
//	bool p_stop, p_initialized;
//};

}
#endif