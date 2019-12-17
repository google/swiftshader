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

#include "XlibSurfaceKHR.hpp"

#include "Vulkan/VkDeviceMemory.hpp"
#include "Vulkan/VkImage.hpp"

namespace vk {

XlibSurfaceKHR::XlibSurfaceKHR(const VkXlibSurfaceCreateInfoKHR *pCreateInfo, void *mem)
    : pDisplay(pCreateInfo->dpy)
    , window(pCreateInfo->window)
{
	int screen = DefaultScreen(pDisplay);
	gc = libX11->XDefaultGC(pDisplay, screen);

	XVisualInfo xVisual;
	Status status = libX11->XMatchVisualInfo(pDisplay, screen, 32, TrueColor, &xVisual);
	bool match = (status != 0 && xVisual.blue_mask == 0xFF);
	visual = match ? xVisual.visual : libX11->XDefaultVisual(pDisplay, screen);
}

void XlibSurfaceKHR::destroySurface(const VkAllocationCallbacks *pAllocator)
{
}

size_t XlibSurfaceKHR::ComputeRequiredAllocationSize(const VkXlibSurfaceCreateInfoKHR *pCreateInfo)
{
	return 0;
}

void XlibSurfaceKHR::getSurfaceCapabilities(VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) const
{
	SurfaceKHR::getSurfaceCapabilities(pSurfaceCapabilities);

	XWindowAttributes attr;
	libX11->XGetWindowAttributes(pDisplay, window, &attr);
	VkExtent2D extent = { static_cast<uint32_t>(attr.width), static_cast<uint32_t>(attr.height) };

	pSurfaceCapabilities->currentExtent = extent;
	pSurfaceCapabilities->minImageExtent = extent;
	pSurfaceCapabilities->maxImageExtent = extent;
}

void XlibSurfaceKHR::attachImage(PresentImage *image)
{
	XWindowAttributes attr;
	libX11->XGetWindowAttributes(pDisplay, window, &attr);

	VkExtent3D extent = image->getImage()->getMipLevelExtent(VK_IMAGE_ASPECT_COLOR_BIT, 0);

	int bytes_per_line = image->getImage()->rowPitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, 0);
	char *buffer = static_cast<char *>(image->getImageMemory()->getOffsetPointer(0));

	XImage *xImage = libX11->XCreateImage(pDisplay, visual, attr.depth, ZPixmap, 0, buffer, extent.width, extent.height, 32, bytes_per_line);

	imageMap[image] = xImage;
}

void XlibSurfaceKHR::detachImage(PresentImage *image)
{
	auto it = imageMap.find(image);
	if(it != imageMap.end())
	{
		XImage *xImage = it->second;
		xImage->data = nullptr;  // the XImage does not actually own the buffer
		XDestroyImage(xImage);
		imageMap.erase(image);
	}
}

VkResult XlibSurfaceKHR::present(PresentImage *image)
{
	auto it = imageMap.find(image);
	if(it != imageMap.end())
	{
		XImage *xImage = it->second;

		if(xImage->data)
		{
			XWindowAttributes attr;
			libX11->XGetWindowAttributes(pDisplay, window, &attr);
			VkExtent2D windowExtent = { static_cast<uint32_t>(attr.width), static_cast<uint32_t>(attr.height) };
			VkExtent3D extent = image->getImage()->getMipLevelExtent(VK_IMAGE_ASPECT_COLOR_BIT, 0);

			if(windowExtent.width != extent.width || windowExtent.height != extent.height)
			{
				return VK_ERROR_OUT_OF_DATE_KHR;
			}

			libX11->XPutImage(pDisplay, window, gc, xImage, 0, 0, 0, 0, extent.width, extent.height);
		}
	}

	return VK_SUCCESS;
}

}  // namespace vk