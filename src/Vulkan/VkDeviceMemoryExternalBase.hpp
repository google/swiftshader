// Copyright 2020 The SwiftShader Authors. All Rights Reserved.
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

#ifndef VK_DEVICE_MEMORY_EXTERNAL_BASE_HPP_
#define VK_DEVICE_MEMORY_EXTERNAL_BASE_HPP_

#include "VkDeviceMemory.hpp"

namespace vk {

// Base abstract interface for a device memory implementation.
class DeviceMemory::ExternalBase
{
public:
	virtual ~ExternalBase() = default;

	// Allocate the memory according to |size|. On success return VK_SUCCESS
	// and sets |*pBuffer|.
	virtual VkResult allocate(size_t size, void **pBuffer) = 0;

	// Deallocate previously allocated memory at |buffer|.
	virtual void deallocate(void *buffer, size_t size) = 0;

	// Return the handle type flag bit supported by this implementation.
	// A value of 0 corresponds to non-external memory.
	virtual VkExternalMemoryHandleTypeFlagBits getFlagBit() const = 0;

	virtual void setDevicePtr(Device *pDevice) {}

#if SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD
	virtual VkResult exportFd(int *pFd) const
	{
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}
#endif

#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
	virtual VkResult exportAndroidHardwareBuffer(struct AHardwareBuffer **pAhb) const
	{
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}

	virtual bool isAndroidHardwareBuffer()
	{
		return false;
	}
#endif

protected:
	ExternalBase() = default;
};

}  // namespace vk

#endif  // VK_DEVICE_MEMORY_EXTERNAL_BASE_HPP_
