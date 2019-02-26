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

#ifndef SWIFTSHADER_VKSWAPCHAINKHR_HPP
#define SWIFTSHADER_VKSWAPCHAINKHR_HPP


#include "Vulkan/VkObject.hpp"
#include "Vulkan/VkImage.hpp"

#include <vector>

namespace vk
{

enum PresentImageStatus
{
	NONEXISTENT, //  Image wasn't made
	AVAILABLE,
	DRAWING,
	PRESENTING,
};

struct PresentImage
{
	VkImage image;
	VkDeviceMemory imageMemory;
	PresentImageStatus imageStatus;
};

class SwapchainKHR : public Object<SwapchainKHR, VkSwapchainKHR>
{
public:
	SwapchainKHR(const VkSwapchainCreateInfoKHR* pCreateInfo, void* mem);
	~SwapchainKHR() = delete;

	void destroy(const VkAllocationCallbacks* pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkSwapchainCreateInfoKHR* pCreateInfo);

	VkResult createImages(VkDevice device);

	uint32_t getImageCount() const;
	VkResult getImages(uint32_t *pSwapchainImageCount, VkImage *pSwapchainImages) const;

private:
	VkSwapchainCreateInfoKHR createInfo;
	std::vector<PresentImage> images;

	void resetImages();
};

static inline SwapchainKHR* Cast(VkSwapchainKHR object)
{
	return reinterpret_cast<SwapchainKHR*>(object);
}

}

#endif //SWIFTSHADER_VKSWAPCHAINKHR_HPP
