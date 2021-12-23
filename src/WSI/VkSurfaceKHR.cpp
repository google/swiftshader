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

#include "Vulkan/VkDestroy.hpp"

#include <algorithm>

namespace {

static const VkSurfaceFormatKHR surfaceFormats[] = {
	{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
	{ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
};

static const VkPresentModeKHR presentModes[] = {
	// FIXME(b/124265819): Make present modes behave correctly. Currently we use XPutImage
	// with no synchronization, which behaves more like VK_PRESENT_MODE_IMMEDIATE_KHR. We
	// should convert to using the Present extension, which allows us to request presentation
	// at particular msc values. Will need a similar solution on Windows - possibly interact
	// with DXGI directly.
	VK_PRESENT_MODE_FIFO_KHR,
	VK_PRESENT_MODE_MAILBOX_KHR,
};

}  // namespace

namespace vk {

VkResult PresentImage::createImage(VkDevice device, const VkImageCreateInfo &createInfo)
{
	VkImage image;
	VkResult status = vkCreateImage(device, &createInfo, nullptr, &image);
	if(status != VK_SUCCESS)
	{
		return status;
	}

	this->image = Cast(image);

	return status;
}

VkResult PresentImage::allocateAndBindImageMemory(VkDevice device, const VkMemoryAllocateInfo &allocateInfo)
{
	ASSERT(image);

	VkDeviceMemory deviceMemory;
	VkResult status = vkAllocateMemory(device, &allocateInfo, nullptr, &deviceMemory);
	if(status != VK_SUCCESS)
	{
		release();
		return status;
	}

	imageMemory = Cast(deviceMemory);
	vkBindImageMemory(device, *image, deviceMemory, 0);
	imageStatus = AVAILABLE;

	return VK_SUCCESS;
}

void PresentImage::release()
{
	if(imageMemory)
	{
		vk::destroy(static_cast<VkDeviceMemory>(*imageMemory), nullptr);
		imageMemory = nullptr;
	}

	if(image)
	{
		vk::destroy(static_cast<VkImage>(*image), nullptr);
		image = nullptr;
	}

	imageStatus = NONEXISTENT;
}

VkImage PresentImage::asVkImage() const
{
	return image ? static_cast<VkImage>(*image) : VkImage({ VK_NULL_HANDLE });
}

uint32_t SurfaceKHR::getSurfaceFormatsCount() const
{
	return static_cast<uint32_t>(sizeof(surfaceFormats) / sizeof(surfaceFormats[0]));
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

	if(*pSurfaceFormatCount < count)
	{
		return VK_INCOMPLETE;
	}

	return VK_SUCCESS;
}

uint32_t SurfaceKHR::getPresentModeCount() const
{
	return static_cast<uint32_t>(sizeof(presentModes) / sizeof(presentModes[0]));
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

	if(*pPresentModeCount < count)
	{
		return VK_INCOMPLETE;
	}

	return VK_SUCCESS;
}

void SurfaceKHR::associateSwapchain(SwapchainKHR *swapchain)
{
	associatedSwapchain = swapchain;
}

void SurfaceKHR::disassociateSwapchain()
{
	associatedSwapchain = nullptr;
}

bool SurfaceKHR::hasAssociatedSwapchain()
{
	return (associatedSwapchain != nullptr);
}

VkResult SurfaceKHR::getPresentRectangles(uint32_t *pRectCount, VkRect2D *pRects) const
{
	if(!pRects)
	{
		*pRectCount = 1;
		return VK_SUCCESS;
	}

	if(*pRectCount < 1)
	{
		return VK_INCOMPLETE;
	}

	VkSurfaceCapabilitiesKHR capabilities;
	getSurfaceCapabilities(&capabilities);

	pRects[0].offset = { 0, 0 };
	pRects[0].extent = capabilities.currentExtent;
	*pRectCount = 1;

	return VK_SUCCESS;
}

void SurfaceKHR::setCommonSurfaceCapabilities(VkSurfaceCapabilitiesKHR *pSurfaceCapabilities)
{
	pSurfaceCapabilities->minImageCount = 1;
	pSurfaceCapabilities->maxImageCount = 0;

	pSurfaceCapabilities->maxImageArrayLayers = 1;

	pSurfaceCapabilities->supportedTransforms = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	pSurfaceCapabilities->currentTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	pSurfaceCapabilities->supportedCompositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	pSurfaceCapabilities->supportedUsageFlags =
	    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
	    VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
	    VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
	    VK_IMAGE_USAGE_TRANSFER_DST_BIT |
	    VK_IMAGE_USAGE_SAMPLED_BIT |
	    VK_IMAGE_USAGE_STORAGE_BIT;
}

}  // namespace vk