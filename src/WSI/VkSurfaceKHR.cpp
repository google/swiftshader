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

#include "VkSurfaceKHR.hpp"

#include <algorithm>

namespace vk
{

void SurfaceKHR::getSurfaceCapabilities(VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) const
{
	pSurfaceCapabilities->minImageCount = 1;
	pSurfaceCapabilities->maxImageCount = 0;

	pSurfaceCapabilities->maxImageArrayLayers = 1;

	pSurfaceCapabilities->supportedTransforms = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	pSurfaceCapabilities->currentTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	pSurfaceCapabilities->supportedCompositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	pSurfaceCapabilities->supportedUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
}

uint32_t SurfaceKHR::getSurfaceFormatsCount() const
{
	return static_cast<uint32_t>(surfaceFormats.size());
}

VkResult SurfaceKHR::getSurfaceFormats(uint32_t *pSurfaceFormatCount, VkSurfaceFormatKHR *pSurfaceFormats) const
{
	uint32_t count = getSurfaceFormatsCount();

	uint32_t i;
	for(i = 0; i < std::min(*pSurfaceFormatCount, count); i++)
	{
		pSurfaceFormats[i] = surfaceFormats[i];
	}

	*pSurfaceFormatCount = i;

	if (*pSurfaceFormatCount < count)
	{
		return VK_INCOMPLETE;
	}

	return VK_SUCCESS;
}

uint32_t SurfaceKHR::getPresentModeCount() const
{
	return static_cast<uint32_t>(presentModes.size());
}


VkResult SurfaceKHR::getPresentModes(uint32_t *pPresentModeCount, VkPresentModeKHR *pPresentModes) const
{
	uint32_t count = getPresentModeCount();

	uint32_t i;
	for(i = 0; i < std::min(*pPresentModeCount, count); i++)
	{
		pPresentModes[i] = presentModes[i];
	}

	*pPresentModeCount = i;

	if (*pPresentModeCount < count)
	{
		return VK_INCOMPLETE;
	}

	return VK_SUCCESS;
}

void SurfaceKHR::associateSwapchain(VkSwapchainKHR swapchain)
{
	associatedSwapchain = swapchain;
}

void SurfaceKHR::disassociateSwapchain()
{
	associatedSwapchain = VK_NULL_HANDLE;
}

VkSwapchainKHR SurfaceKHR::getAssociatedSwapchain()
{
	return associatedSwapchain;
}

}