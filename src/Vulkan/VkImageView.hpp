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

#ifndef VK_IMAGE_VIEW_HPP_
#define VK_IMAGE_VIEW_HPP_

#include "VkDebug.hpp"
#include "VkObject.hpp"
#include "VkImage.hpp"

namespace vk
{

class ImageView : public Object<ImageView, VkImageView>
{
public:
	ImageView(const VkImageViewCreateInfo* pCreateInfo, void* mem);
	~ImageView() = delete;
	void destroy(const VkAllocationCallbacks* pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkImageViewCreateInfo* pCreateInfo);

	void clear(const VkClearValue& clearValues, const VkRect2D& renderArea);

private:
	bool                       imageTypesMatch(VkImageType imageType) const;

	Image*                     image = nullptr;
	VkImageViewType            viewType = VK_IMAGE_VIEW_TYPE_2D;
	VkFormat                   format = VK_FORMAT_UNDEFINED;
	VkComponentMapping         components = {};
	VkImageSubresourceRange    subresourceRange = {};
};

static inline ImageView* Cast(VkImageView object)
{
	return reinterpret_cast<ImageView*>(object);
}

} // namespace vk

#endif // VK_IMAGE_VIEW_HPP_