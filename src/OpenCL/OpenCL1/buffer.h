//TODO: copyrights

#ifndef __CPU_BUFFER_H__
#define __CPU_BUFFER_H__

#include "device_interface.h"

namespace Devices
{

	class CPUDevice;
	class MemObject;

	/**
	* \brief CPU implementation of \c Coal::MemObject
	*
	* This class is responsible of the actual allocation of buffer objects, using
	* \c malloc() or by reusing a given \c host_ptr.
	*/
	class CPUBuffer : public DeviceBuffer
	{
	public:
		/**
		* \brief Constructor
		* \param device Device for which the buffer is allocated
		* \param buffer \c Coal::MemObject holding information about the buffer
		* \param rs return code (\c CL_SUCCESS if all is good)
		*/
		CPUBuffer(CPUDevice *device, MemObject *buffer, cl_int *rs);
		~CPUBuffer();

		bool allocate();
		DeviceInterface *device() const;
		void *data() const;                 /*!< \brief Pointer to the buffer's data */
		void *nativeGlobalPointer() const;
		bool allocated() const;

	private:
		CPUDevice *p_device;
		MemObject *p_buffer;
		void *p_data;
		bool p_data_malloced;
	};

}

#endif
