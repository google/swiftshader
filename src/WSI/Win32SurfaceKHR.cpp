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

#include "Win32SurfaceKHR.hpp"

#include "System/Debug.hpp"
#include "Vulkan/VkDeviceMemory.hpp"

#include <string.h>

namespace {
VkExtent2D getWindowSize(HWND hwnd)
{
	RECT clientRect = {};
	BOOL status = GetClientRect(hwnd, &clientRect);
	ASSERT(status != 0);

	int windowWidth = clientRect.right - clientRect.left;
	int windowHeight = clientRect.bottom - clientRect.top;

	return { static_cast<uint32_t>(windowWidth), static_cast<uint32_t>(windowHeight) };
}
}  // namespace

namespace vk {

Win32SurfaceKHR::Win32SurfaceKHR(const VkWin32SurfaceCreateInfoKHR *pCreateInfo, void *mem)
    : hwnd(pCreateInfo->hwnd)
{
	ASSERT(IsWindow(hwnd) == TRUE);
	windowContext = GetDC(hwnd);
	bitmapContext = CreateCompatibleDC(windowContext);
	lazyCreateFrameBuffer();
}

void Win32SurfaceKHR::destroySurface(const VkAllocationCallbacks *pAllocator)
{
	destroyFrameBuffer();
	ReleaseDC(hwnd, windowContext);
	DeleteDC(bitmapContext);
}

size_t Win32SurfaceKHR::ComputeRequiredAllocationSize(const VkWin32SurfaceCreateInfoKHR *pCreateInfo)
{
	return 0;
}

void Win32SurfaceKHR::getSurfaceCapabilities(VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) const
{
	SurfaceKHR::getSurfaceCapabilities(pSurfaceCapabilities);
	VkExtent2D extent = getWindowSize(hwnd);
	pSurfaceCapabilities->currentExtent = extent;
	pSurfaceCapabilities->minImageExtent = extent;
	pSurfaceCapabilities->maxImageExtent = extent;
}

void Win32SurfaceKHR::attachImage(PresentImage *image)
{
	// Nothing to do here, the current implementation based on GDI blits on
	// present instead of associating the image with the surface.
}

void Win32SurfaceKHR::detachImage(PresentImage *image)
{
	// Nothing to do here, the current implementation based on GDI blits on
	// present instead of associating the image with the surface.
}

VkResult Win32SurfaceKHR::present(PresentImage *image)
{
	// Recreate frame buffer in case window size has changed
	lazyCreateFrameBuffer();

	if(!framebuffer)
	{
		// e.g. window width or height is 0
		return VK_SUCCESS;
	}

	VkExtent3D extent = image->getImage()->getMipLevelExtent(VK_IMAGE_ASPECT_COLOR_BIT, 0);

	if(windowExtent.width != extent.width || windowExtent.height != extent.height)
	{
		return VK_ERROR_OUT_OF_DATE_KHR;
	}

	VkImageSubresourceLayers subresourceLayers{};
	subresourceLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceLayers.layerCount = 1;

	image->getImage()->blitToBuffer(subresourceLayers, VkOffset3D{}, extent, reinterpret_cast<uint8_t *>(framebuffer), bitmapRowPitch, 0);

	StretchBlt(windowContext, 0, 0, extent.width, extent.height, bitmapContext, 0, 0, extent.width, extent.height, SRCCOPY);

	return VK_SUCCESS;
}

void Win32SurfaceKHR::lazyCreateFrameBuffer()
{
	auto currWindowExtent = getWindowSize(hwnd);
	if(currWindowExtent.width == windowExtent.width && currWindowExtent.height == windowExtent.height)
	{
		return;
	}

	windowExtent = currWindowExtent;

	if(framebuffer)
	{
		destroyFrameBuffer();
	}

	if(windowExtent.width == 0 || windowExtent.height == 0)
	{
		return;
	}

	BITMAPINFO bitmapInfo = {};
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biHeight = -static_cast<LONG>(windowExtent.height);  // Negative for top-down DIB, origin in upper-left corner
	bitmapInfo.bmiHeader.biWidth = windowExtent.width;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	bitmap = CreateDIBSection(bitmapContext, &bitmapInfo, DIB_RGB_COLORS, &framebuffer, 0, 0);
	ASSERT(bitmap != NULL);
	SelectObject(bitmapContext, bitmap);

	BITMAP header;
	int status = GetObject(bitmap, sizeof(BITMAP), &header);
	ASSERT(status != 0);
	bitmapRowPitch = static_cast<int>(header.bmWidthBytes);
}

void Win32SurfaceKHR::destroyFrameBuffer()
{
	SelectObject(bitmapContext, NULL);
	DeleteObject(bitmap);
	bitmap = {};
	bitmapRowPitch = 0;
	framebuffer = nullptr;
}

}  // namespace vk