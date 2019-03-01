// Copyright 2018 The SwiftShader Authors. All Rights Reserved.
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

#ifndef SWIFTSHADER_VKSURFACEKHR_HPP_
#define SWIFTSHADER_VKSURFACEKHR_HPP_

#include "Vulkan/VkObject.hpp"
#include <vulkan/vulkan.h>
#include <vector>

namespace vk
{

class SurfaceKHR
{
public:
	operator VkSurfaceKHR()
	{
		return reinterpret_cast<VkSurfaceKHR>(this);
	}

	void destroy(const VkAllocationCallbacks* pAllocator)
	{
		destroySurface(pAllocator);
	}

	virtual void destroySurface(const VkAllocationCallbacks* pAllocator) = 0;

	virtual void getSurfaceCapabilities(VkSurfaceCapabilitiesKHR* pSurfaceCapabilities) const;

	uint32_t getSurfaceFormatsCount() const;
	VkResult getSurfaceFormats(uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats) const;

	uint32_t getPresentModeCount() const;
	VkResult getPresentModes(uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes) const;

private:
	const std::vector<VkSurfaceFormatKHR> surfaceFormats =
	{
		{VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
	};

	const std::vector<VkPresentModeKHR> presentModes =
	{
		VK_PRESENT_MODE_FIFO_KHR,
	};
};

static inline SurfaceKHR* Cast(VkSurfaceKHR object)
{
	return reinterpret_cast<SurfaceKHR*>(object);
}

}

#endif //SWIFTSHADER_VKSURFACEKHR_HPP_
