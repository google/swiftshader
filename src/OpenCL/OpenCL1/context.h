//TODO: copyrights

#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "object.h"
#include "opencl.h"

namespace Devices
{

	class DeviceInterface;

	/**
	* \brief OpenCL context
	*
	* This class is the root of all OpenCL objects, except \c Coal::DeviceInterface.
	*/
	class Context : public Object
	{
	public:
		/**
		* \brief Constructor
		* \param properties properties of the context
		* \param num_devices number of devices that will be used
		* \param devices \c Coal::DeviceInterface to be used
		* \param pfn_notify function to  call when an error arises, to give
		*        more detail
		* \param user_data user data to pass to \p pfn_notify
		* \param errcode_ret return code
		*/
		Context(const cl_context_properties *properties,
			cl_uint num_devices,
			const cl_device_id *devices,
			void (CL_CALLBACK *pfn_notify)(const char *, const void *,
			size_t, void *),
			void *user_data,
			cl_int *errcode_ret);
		~Context();

		/**
		* \brief Info about the context
		* \copydetails Coal::DeviceInterface::info
		*/
		cl_int info(cl_context_info param_name,
			size_t param_value_size,
			void *param_value,
			size_t *param_value_size_ret) const;

		/**
		* \brief Check that this context contains a given \p device
		* \param device device to check
		* \return whether this context contains \p device
		*/
		bool hasDevice(DeviceInterface *device) const;

	private:
		cl_context_properties *p_properties;
		void (CL_CALLBACK *p_pfn_notify)(const char *, const void *,
			size_t, void *);
		void *p_user_data;

		DeviceInterface **p_devices;
		unsigned int p_num_devices, p_props_len;
		cl_platform_id p_platform;
		cl_context_properties wgl_handle;
		cl_context_properties opengl_context_handle;
	};

}

struct _cl_context : public Devices::Context
{};

#endif
