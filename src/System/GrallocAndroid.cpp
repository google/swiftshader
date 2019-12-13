// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "GrallocAndroid.hpp"
#include "Debug.hpp"

#ifdef HAVE_GRALLOC1
#	include <sync/sync.h>
#endif
#ifdef HAVE_GRALLOC3
using V3Error = android::hardware::graphics::mapper::V3_0::Error;
using V3Mapper = android::hardware::graphics::mapper::V3_0::IMapper;
using android::hardware::hidl_handle;
#endif
#ifdef HAVE_GRALLOC4
using V4Error = android::hardware::graphics::mapper::V4_0::Error;
using V4Mapper = android::hardware::graphics::mapper::V4_0::IMapper;
using android::hardware::hidl_handle;
#endif

GrallocModule *GrallocModule::getInstance()
{
	static GrallocModule instance;
	return &instance;
}

GrallocModule::GrallocModule()
{
#ifdef HAVE_GRALLOC4
	m_gralloc4_mapper = V4Mapper::getService();
	if(m_gralloc4_mapper != nullptr)
	{
		return;
	}
#endif

#ifdef HAVE_GRALLOC3
	m_gralloc3_mapper = V3Mapper::getService();
	if(m_gralloc3_mapper != nullptr)
	{
		return;
	}
#endif

	const hw_module_t *module = nullptr;
	hw_get_module(GRALLOC_HARDWARE_MODULE_ID, &module);

	m_major_version = (module->module_api_version >> 8) & 0xff;
	switch(m_major_version)
	{
	case 0:
		m_module = reinterpret_cast<const gralloc_module_t *>(module);
		break;
	case 1:
#ifdef HAVE_GRALLOC1
		gralloc1_open(module, &m_gralloc1_device);
		m_gralloc1_lock = (GRALLOC1_PFN_LOCK)m_gralloc1_device->getFunction(m_gralloc1_device, GRALLOC1_FUNCTION_LOCK);
		m_gralloc1_unlock = (GRALLOC1_PFN_UNLOCK)m_gralloc1_device->getFunction(m_gralloc1_device, GRALLOC1_FUNCTION_UNLOCK);
		break;
#endif
	default:
		TRACE("unknown gralloc major version (%d)", m_major_version);
		break;
	}
}

int GrallocModule::import(buffer_handle_t handle, buffer_handle_t *imported_handle)
{
#ifdef HAVE_GRALLOC4
	if(m_gralloc4_mapper != nullptr)
	{
		V4Error error;
		auto ret = m_gralloc4_mapper->importBuffer(handle,
		                                           [&](const auto &tmp_err, const auto &tmp_buf) {
			                                           error = tmp_err;
			                                           if(error == V4Error::NONE)
			                                           {
				                                           *imported_handle = static_cast<buffer_handle_t>(tmp_buf);
			                                           }
		                                           });
		return ret.isOk() && error == V4Error::NONE ? 0 : -1;
	}
#endif

#ifdef HAVE_GRALLOC3
	if(m_gralloc3_mapper != nullptr)
	{
		V3Error error;
		auto ret = m_gralloc3_mapper->importBuffer(handle,
		                                           [&](const auto &tmp_err, const auto &tmp_buf) {
			                                           error = tmp_err;
			                                           if(error == V3Error::NONE)
			                                           {
				                                           *imported_handle = static_cast<buffer_handle_t>(tmp_buf);
			                                           }
		                                           });
		return ret.isOk() && error == V3Error::NONE ? 0 : -1;
	}
#endif

	*imported_handle = handle;
	return 0;
}

int GrallocModule::release(buffer_handle_t handle)
{
#ifdef HAVE_GRALLOC4
	if(m_gralloc4_mapper != nullptr)
	{
		native_handle_t *native_handle = const_cast<native_handle_t *>(handle);
		return m_gralloc4_mapper->freeBuffer(native_handle).isOk() ? 0 : 1;
	}
#endif

#ifdef HAVE_GRALLOC3
	if(m_gralloc3_mapper != nullptr)
	{
		native_handle_t *native_handle = const_cast<native_handle_t *>(handle);
		return m_gralloc3_mapper->freeBuffer(native_handle).isOk() ? 0 : 1;
	}
#endif

	return 0;
}

int GrallocModule::lock(buffer_handle_t handle, int usage, int left, int top, int width, int height, void **vaddr)
{
#ifdef HAVE_GRALLOC4
	if(m_gralloc4_mapper != nullptr)
	{
		native_handle_t *native_handle = const_cast<native_handle_t *>(handle);

		V4Mapper::Rect rect;
		rect.left = left;
		rect.top = top;
		rect.width = width;
		rect.height = height;

		hidl_handle empty_fence_handle;

		V4Error error;
		auto ret = m_gralloc4_mapper->lock(native_handle, usage, rect, empty_fence_handle,
		                                   [&](const auto &tmp_err, const auto &tmp_vaddr) {
			                                   error = tmp_err;
			                                   if(tmp_err == V4Error::NONE)
			                                   {
				                                   *vaddr = tmp_vaddr;
			                                   }
		                                   });
		return ret.isOk() && error == V4Error::NONE ? 0 : -1;
	}
#endif

#ifdef HAVE_GRALLOC3
	if(m_gralloc3_mapper != nullptr)
	{
		native_handle_t *native_handle = const_cast<native_handle_t *>(handle);

		V3Mapper::Rect rect;
		rect.left = left;
		rect.top = top;
		rect.width = width;
		rect.height = height;

		hidl_handle empty_fence_handle;

		V3Error error;
		auto ret = m_gralloc3_mapper->lock(native_handle, usage, rect, empty_fence_handle,
		                                   [&](const auto &tmp_err,
		                                       const auto &tmp_vaddr,
		                                       const auto & /*bytes_per_pixel*/,
		                                       const auto & /*bytes_per_stride*/) {
			                                   error = tmp_err;
			                                   if(tmp_err == V3Error::NONE)
			                                   {
				                                   *vaddr = tmp_vaddr;
			                                   }
		                                   });
		return ret.isOk() && error == V3Error::NONE ? 0 : -1;
	}
#endif

	switch(m_major_version)
	{
	case 0:
		{
			return m_module->lock(m_module, handle, usage, left, top, width, height, vaddr);
		}
	case 1:
#ifdef HAVE_GRALLOC1
		{
			gralloc1_rect_t outRect{};
			outRect.left = left;
			outRect.top = top;
			outRect.width = width;
			outRect.height = height;
			return m_gralloc1_lock(m_gralloc1_device, handle, usage, usage, &outRect, vaddr, -1);
		}
#endif
	default:
		{
			TRACE("no gralloc module to lock");
			return -1;
		}
	}
}

int GrallocModule::unlock(buffer_handle_t handle)
{
#ifdef HAVE_GRALLOC4
	if(m_gralloc4_mapper != nullptr)
	{
		native_handle_t *native_handle = const_cast<native_handle_t *>(handle);

		V4Error error;
		auto ret = m_gralloc4_mapper->unlock(native_handle,
		                                     [&](const auto &tmp_err, const auto &) {
			                                     error = tmp_err;
		                                     });
		return ret.isOk() && error == V4Error::NONE ? 0 : -1;
	}
#endif

#ifdef HAVE_GRALLOC3
	if(m_gralloc3_mapper != nullptr)
	{
		native_handle_t *native_handle = const_cast<native_handle_t *>(handle);

		V3Error error;
		auto ret = m_gralloc3_mapper->unlock(native_handle,
		                                     [&](const auto &tmp_err, const auto &) {
			                                     error = tmp_err;
		                                     });
		return ret.isOk() && error == V3Error::NONE ? 0 : -1;
	}
#endif

	switch(m_major_version)
	{
	case 0:
		{
			return m_module->unlock(m_module, handle);
		}
	case 1:
#ifdef HAVE_GRALLOC1
		{
			int32_t fenceFd = -1;
			int error = m_gralloc1_unlock(m_gralloc1_device, handle, &fenceFd);
			if(!error)
			{
				sync_wait(fenceFd, -1);
				close(fenceFd);
			}
			return error;
		}
#endif
	default:
		{
			TRACE("no gralloc module to unlock");
			return -1;
		}
	}
}
