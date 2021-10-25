// Copyright 2018 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "XcbSurfaceKHR.hpp"

#include "libXCB.hpp"
#include "Vulkan/VkDeviceMemory.hpp"
#include "Vulkan/VkImage.hpp"

#include <memory>

namespace vk {

bool getWindowSizeAndDepth(xcb_connection_t *connection, xcb_window_t window, VkExtent2D *windowExtent, int *depth)
{
	auto cookie = libXCB->xcb_get_geometry(connection, window);
	if(auto *geom = libXCB->xcb_get_geometry_reply(connection, cookie, nullptr))
	{
		windowExtent->width = static_cast<uint32_t>(geom->width);
		windowExtent->height = static_cast<uint32_t>(geom->height);
		*depth = static_cast<int>(geom->depth);
		free(geom);
		return true;
	}
	return false;
}

bool XcbSurfaceKHR::isSupported()
{
	return libXCB.isPresent();
}

XcbSurfaceKHR::XcbSurfaceKHR(const VkXcbSurfaceCreateInfoKHR *pCreateInfo, void *mem)
    : connection(pCreateInfo->connection)
    , window(pCreateInfo->window)
{
	ASSERT(isSupported());
}

void XcbSurfaceKHR::destroySurface(const VkAllocationCallbacks *pAllocator)
{
}

size_t XcbSurfaceKHR::ComputeRequiredAllocationSize(const VkXcbSurfaceCreateInfoKHR *pCreateInfo)
{
	return 0;
}

VkResult XcbSurfaceKHR::getSurfaceCapabilities(VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) const
{
	setCommonSurfaceCapabilities(pSurfaceCapabilities);

	VkExtent2D extent;
	int depth;
	if(!getWindowSizeAndDepth(connection, window, &extent, &depth))
	{
		return VK_ERROR_SURFACE_LOST_KHR;
	}

	pSurfaceCapabilities->currentExtent = extent;
	pSurfaceCapabilities->minImageExtent = extent;
	pSurfaceCapabilities->maxImageExtent = extent;
	return VK_SUCCESS;
}

void XcbSurfaceKHR::attachImage(PresentImage *image)
{
	auto gc = libXCB->xcb_generate_id(connection);

	uint32_t values[2] = { 0, 0xffffffff };
	libXCB->xcb_create_gc(connection, gc, window, XCB_GC_FOREGROUND | XCB_GC_BACKGROUND, values);

	graphicsContexts[image] = gc;
}

void XcbSurfaceKHR::detachImage(PresentImage *image)
{
	auto it = graphicsContexts.find(image);
	if(it != graphicsContexts.end())
	{
		libXCB->xcb_free_gc(connection, it->second);
		graphicsContexts.erase(it);
	}
}

VkResult XcbSurfaceKHR::present(PresentImage *image)
{
	auto it = graphicsContexts.find(image);
	if(it != graphicsContexts.end())
	{
		VkExtent2D windowExtent;
		int depth;
		if(!getWindowSizeAndDepth(connection, window, &windowExtent, &depth))
		{
			return VK_ERROR_SURFACE_LOST_KHR;
		}

		const VkExtent3D &extent = image->getImage()->getExtent();

		if(windowExtent.width != extent.width || windowExtent.height != extent.height)
		{
			return VK_ERROR_OUT_OF_DATE_KHR;
		}

		// TODO: Convert image if not RGB888.
		int stride = image->getImage()->rowPitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, 0);
		auto buffer = reinterpret_cast<uint8_t *>(image->getImageMemory()->getOffsetPointer(0));
		size_t bufferSize = extent.height * stride;
		libXCB->xcb_put_image(
		    connection,
		    XCB_IMAGE_FORMAT_Z_PIXMAP,
		    window,
		    it->second,
		    extent.width,
		    extent.height,
		    0, 0,  // dst x, y
		    0,     // left_pad
		    depth,
		    bufferSize,  // data_len
		    buffer       // data
		);

		libXCB->xcb_flush(connection);
	}

	return VK_SUCCESS;
}

}  // namespace vk