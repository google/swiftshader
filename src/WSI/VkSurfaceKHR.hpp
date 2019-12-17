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

#include "Vulkan/VkImage.hpp"
#include "Vulkan/VkObject.hpp"
#include <Vulkan/VulkanPlatform.h>

#include <vector>

namespace vk {

enum PresentImageStatus
{
	NONEXISTENT,  // Image wasn't created
	AVAILABLE,
	DRAWING,
	PRESENTING,
};

class DeviceMemory;
class Image;
class SwapchainKHR;

class PresentImage
{
public:
	VkResult allocateImage(VkDevice device, const VkImageCreateInfo &createInfo);
	VkResult allocateAndBindImageMemory(VkDevice device, const VkMemoryAllocateInfo &allocateInfo);
	void clear();
	VkImage asVkImage() const;

	const Image *getImage() const { return image; }
	DeviceMemory *getImageMemory() const { return imageMemory; }
	bool isAvailable() const { return (imageStatus == AVAILABLE); }
	bool exists() const { return (imageStatus != NONEXISTENT); }
	void setStatus(PresentImageStatus status) { imageStatus = status; }

private:
	Image *image = nullptr;
	DeviceMemory *imageMemory = nullptr;
	PresentImageStatus imageStatus = NONEXISTENT;
};

class SurfaceKHR
{
public:
	virtual ~SurfaceKHR() = default;

	operator VkSurfaceKHR()
	{
		return vk::TtoVkT<SurfaceKHR, VkSurfaceKHR>(this);
	}

	static inline SurfaceKHR *Cast(VkSurfaceKHR object)
	{
		return vk::VkTtoT<SurfaceKHR, VkSurfaceKHR>(object);
	}

	void destroy(const VkAllocationCallbacks *pAllocator)
	{
		destroySurface(pAllocator);
	}

	virtual void destroySurface(const VkAllocationCallbacks *pAllocator) = 0;

	virtual void getSurfaceCapabilities(VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) const;

	uint32_t getSurfaceFormatsCount() const;
	VkResult getSurfaceFormats(uint32_t *pSurfaceFormatCount, VkSurfaceFormatKHR *pSurfaceFormats) const;

	uint32_t getPresentModeCount() const;
	VkResult getPresentModes(uint32_t *pPresentModeCount, VkPresentModeKHR *pPresentModes) const;

	VkResult getPresentRectangles(uint32_t *pRectCount, VkRect2D *pRects) const;

	virtual void attachImage(PresentImage *image) = 0;
	virtual void detachImage(PresentImage *image) = 0;
	virtual VkResult present(PresentImage *image) = 0;

	void associateSwapchain(SwapchainKHR *swapchain);
	void disassociateSwapchain();
	bool hasAssociatedSwapchain();

private:
	SwapchainKHR *associatedSwapchain = nullptr;
};

static inline SurfaceKHR *Cast(VkSurfaceKHR object)
{
	return SurfaceKHR::Cast(object);
}

}  // namespace vk

#endif  //SWIFTSHADER_VKSURFACEKHR_HPP_
