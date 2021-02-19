// Copyright 2021 The SwiftShader Authors. All Rights Reserved.
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

#include "HeadlessSurfaceKHR.hpp"

namespace vk {

HeadlessSurfaceKHR::HeadlessSurfaceKHR(const VkHeadlessSurfaceCreateInfoEXT *pCreateInfo, void *mem)
{
}

size_t HeadlessSurfaceKHR::ComputeRequiredAllocationSize(const VkHeadlessSurfaceCreateInfoEXT *pCreateInfo)
{
	return 0;
}

void HeadlessSurfaceKHR::destroySurface(const VkAllocationCallbacks *pAllocator)
{
}

VkResult HeadlessSurfaceKHR::getSurfaceCapabilities(VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) const
{
	setCommonSurfaceCapabilities(pSurfaceCapabilities);
	// We could return {0,0} here, but as this type of surface is useful for testing, we return a more
	// common size here so that buffers like Swapchain images get created with reasonable sizes, and
	// so that drawing actually exercises common code paths.
	VkExtent2D extent{ 1280, 720 };
	pSurfaceCapabilities->currentExtent = extent;
	pSurfaceCapabilities->minImageExtent = extent;
	pSurfaceCapabilities->maxImageExtent = extent;
	return VK_SUCCESS;
}

void HeadlessSurfaceKHR::attachImage(PresentImage *image)
{
}

void HeadlessSurfaceKHR::detachImage(PresentImage *image)
{
}

VkResult HeadlessSurfaceKHR::present(PresentImage *image)
{
	return VK_SUCCESS;
}

}  // namespace vk