// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
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

#include "VkSwapchainKHR.hpp"

#include "Vulkan/VkImage.hpp"
#include "Vulkan/VkDeviceMemory.hpp"
#include "Vulkan/VkDestroy.h"

#include <algorithm>

namespace vk
{

SwapchainKHR::SwapchainKHR(const VkSwapchainCreateInfoKHR *pCreateInfo, void *mem) :
	createInfo(*pCreateInfo),
	retired(false)
{
	images.resize(pCreateInfo->minImageCount);
	resetImages();
}

void SwapchainKHR::destroy(const VkAllocationCallbacks *pAllocator)
{
	for(auto& currentImage : images)
	{
		if (currentImage.imageStatus != NONEXISTENT)
		{
			vk::Cast(createInfo.surface)->detachImage(&currentImage);
			vk::destroy(currentImage.imageMemory, pAllocator);
			vk::destroy(currentImage.image, pAllocator);

			currentImage.imageStatus = NONEXISTENT;
		}
	}

	if(!retired)
	{
		vk::Cast(createInfo.surface)->disassociateSwapchain();
	}
}

size_t SwapchainKHR::ComputeRequiredAllocationSize(const VkSwapchainCreateInfoKHR *pCreateInfo)
{
	return 0;
}

void SwapchainKHR::retire()
{
	if(!retired)
	{
		retired = true;
		vk::Cast(createInfo.surface)->disassociateSwapchain();

		for(auto& currentImage : images)
		{
			if(currentImage.imageStatus == AVAILABLE)
			{
				vk::Cast(createInfo.surface)->detachImage(&currentImage);
				vk::destroy(currentImage.imageMemory, nullptr);
				vk::destroy(currentImage.image, nullptr);

				currentImage.imageStatus = NONEXISTENT;
			}
		}
	}
}

void SwapchainKHR::resetImages()
{
	for(auto& currentImage : images)
	{
		currentImage.image = VK_NULL_HANDLE;
		currentImage.imageMemory = VK_NULL_HANDLE;
		currentImage.imageStatus = NONEXISTENT;
	}
}

VkResult SwapchainKHR::createImages(VkDevice device)
{
	resetImages();

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

	if(createInfo.flags & VK_SWAPCHAIN_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR)
	{
		imageInfo.flags |= VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT;
	}

	if(createInfo.flags & VK_SWAPCHAIN_CREATE_PROTECTED_BIT_KHR)
	{
		imageInfo.flags |= VK_IMAGE_CREATE_PROTECTED_BIT;
	}

	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = createInfo.imageFormat;
	imageInfo.extent.height = createInfo.imageExtent.height;
	imageInfo.extent.width = createInfo.imageExtent.width;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = createInfo.imageArrayLayers;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = createInfo.imageUsage;
	imageInfo.sharingMode = createInfo.imageSharingMode;
	imageInfo.pQueueFamilyIndices = createInfo.pQueueFamilyIndices;
	imageInfo.queueFamilyIndexCount = createInfo.queueFamilyIndexCount;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkResult status;
	for(auto& currentImage : images)
	{
		status = vkCreateImage(device, &imageInfo, nullptr, &currentImage.image);
		if(status != VK_SUCCESS)
		{
			return status;
		}

		VkMemoryRequirements memRequirements = vk::Cast(currentImage.image)->getMemoryRequirements();

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = 0;

		status = vkAllocateMemory(device, &allocInfo, nullptr, &currentImage.imageMemory);
		if(status != VK_SUCCESS)
		{
			return status;
		}

		vkBindImageMemory(device, currentImage.image, currentImage.imageMemory, 0);

		currentImage.imageStatus = AVAILABLE;

		vk::Cast(createInfo.surface)->attachImage(&currentImage);
	}

	return VK_SUCCESS;
}

uint32_t SwapchainKHR::getImageCount() const
{
	return static_cast<uint32_t >(images.size());
}

VkResult SwapchainKHR::getImages(uint32_t *pSwapchainImageCount, VkImage *pSwapchainImages) const
{
	uint32_t count = getImageCount();

	uint32_t i;
	for (i = 0; i < std::min(*pSwapchainImageCount, count); i++)
	{
		pSwapchainImages[i] = images[i].image;
	}

	*pSwapchainImageCount = i;

	if (*pSwapchainImageCount < count)
	{
		return VK_INCOMPLETE;
	}

	return VK_SUCCESS;
}

VkResult SwapchainKHR::getNextImage(uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t *pImageIndex)
{
	for(uint32_t i = 0; i < getImageCount(); i++)
	{
		PresentImage& currentImage = images[i];
		if(currentImage.imageStatus == AVAILABLE)
		{
			currentImage.imageStatus = DRAWING;
			*pImageIndex = i;

			if(semaphore)
			{
				vk::Cast(semaphore)->signal();
			}

			if(fence)
			{
				vk::Cast(fence)->signal();
			}

			return VK_SUCCESS;
		}
	}

	return VK_NOT_READY;
}

void SwapchainKHR::present(uint32_t index)
{
	auto & image = images[index];
	image.imageStatus = PRESENTING;
	vk::Cast(createInfo.surface)->present(&image);
	image.imageStatus = AVAILABLE;

	if(retired)
	{
		vk::Cast(createInfo.surface)->detachImage(&image);
		vk::destroy(image.imageMemory, nullptr);
		vk::destroy(image.image, nullptr);

		image.imageStatus = NONEXISTENT;
	}
}

}