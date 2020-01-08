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

#include "Vulkan/VkDeviceMemory.hpp"
#include "Vulkan/VkImage.hpp"

#include "System/SharedLibrary.hpp"

#include <memory>

namespace {

template<typename FPTR>
void getFuncAddress(void *lib, const char *name, FPTR *out)
{
	*out = reinterpret_cast<FPTR>(getProcAddress(lib, name));
}

struct LibXcbExports
{
	LibXcbExports(void *lib)
	{
		getFuncAddress(lib, "xcb_create_gc", &xcb_create_gc);
		getFuncAddress(lib, "xcb_flush", &xcb_flush);
		getFuncAddress(lib, "xcb_free_gc", &xcb_free_gc);
		getFuncAddress(lib, "xcb_generate_id", &xcb_generate_id);
		getFuncAddress(lib, "xcb_get_geometry", &xcb_get_geometry);
		getFuncAddress(lib, "xcb_get_geometry_reply", &xcb_get_geometry_reply);
		getFuncAddress(lib, "xcb_put_image", &xcb_put_image);
	}

	xcb_void_cookie_t (*xcb_create_gc)(xcb_connection_t *c, xcb_gcontext_t cid, xcb_drawable_t drawable, uint32_t value_mask, const void *value_list);
	int (*xcb_flush)(xcb_connection_t *c);
	xcb_void_cookie_t (*xcb_free_gc)(xcb_connection_t *c, xcb_gcontext_t gc);
	uint32_t (*xcb_generate_id)(xcb_connection_t *c);
	xcb_get_geometry_cookie_t (*xcb_get_geometry)(xcb_connection_t *c, xcb_drawable_t drawable);
	xcb_get_geometry_reply_t *(*xcb_get_geometry_reply)(xcb_connection_t *c, xcb_get_geometry_cookie_t cookie, xcb_generic_error_t **e);
	xcb_void_cookie_t (*xcb_put_image)(xcb_connection_t *c, uint8_t format, xcb_drawable_t drawable, xcb_gcontext_t gc, uint16_t width, uint16_t height, int16_t dst_x, int16_t dst_y, uint8_t left_pad, uint8_t depth, uint32_t data_len, const uint8_t *data);
};

class LibXcb
{
public:
	operator bool()
	{
		return loadExports();
	}

	LibXcbExports *operator->()
	{
		return loadExports();
	}

private:
	LibXcbExports *loadExports()
	{
		static auto exports = [] {
			if(getProcAddress(RTLD_DEFAULT, "xcb_create_gc"))
			{
				return std::make_unique<LibXcbExports>(RTLD_DEFAULT);
			}

			if(auto lib = loadLibrary("libxcb.so.1"))
			{
				return std::make_unique<LibXcbExports>(lib);
			}

			return std::unique_ptr<LibXcbExports>();
		}();

		return exports.get();
	}
};

LibXcb libXcb;

}  // anonymous namespace

namespace vk {

XcbSurfaceKHR::XcbSurfaceKHR(const VkXcbSurfaceCreateInfoKHR *pCreateInfo, void *mem)
    : connection(pCreateInfo->connection)
    , window(pCreateInfo->window)
{
}

void XcbSurfaceKHR::destroySurface(const VkAllocationCallbacks *pAllocator)
{
}

size_t XcbSurfaceKHR::ComputeRequiredAllocationSize(const VkXcbSurfaceCreateInfoKHR *pCreateInfo)
{
	return 0;
}

void XcbSurfaceKHR::getSurfaceCapabilities(VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) const
{
	SurfaceKHR::getSurfaceCapabilities(pSurfaceCapabilities);

	auto geom = libXcb->xcb_get_geometry_reply(connection, libXcb->xcb_get_geometry(connection, window), nullptr);
	VkExtent2D extent = { static_cast<uint32_t>(geom->width), static_cast<uint32_t>(geom->height) };
	free(geom);

	pSurfaceCapabilities->currentExtent = extent;
	pSurfaceCapabilities->minImageExtent = extent;
	pSurfaceCapabilities->maxImageExtent = extent;
}

void XcbSurfaceKHR::attachImage(PresentImage *image)
{
	auto gc = libXcb->xcb_generate_id(connection);

	uint32_t values[2] = { 0, 0xffffffff };
	libXcb->xcb_create_gc(connection, gc, window, XCB_GC_FOREGROUND | XCB_GC_BACKGROUND, values);

	graphicsContexts[image] = gc;
}

void XcbSurfaceKHR::detachImage(PresentImage *image)
{
	auto it = graphicsContexts.find(image);
	if(it != graphicsContexts.end())
	{
		libXcb->xcb_free_gc(connection, it->second);
		graphicsContexts.erase(image);
	}
}

VkResult XcbSurfaceKHR::present(PresentImage *image)
{
	auto it = graphicsContexts.find(image);
	if(it != graphicsContexts.end())
	{
		auto geom = libXcb->xcb_get_geometry_reply(connection, libXcb->xcb_get_geometry(connection, window), nullptr);
		VkExtent2D windowExtent = { static_cast<uint32_t>(geom->width), static_cast<uint32_t>(geom->height) };
		free(geom);
		VkExtent3D extent = image->getImage()->getMipLevelExtent(VK_IMAGE_ASPECT_COLOR_BIT, 0);

		if(windowExtent.width != extent.width || windowExtent.height != extent.height)
		{
			return VK_ERROR_OUT_OF_DATE_KHR;
		}

		// TODO: Convert image if not RGB888.
		int stride = image->getImage()->rowPitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, 0);
		auto buffer = reinterpret_cast<uint8_t *>(image->getImageMemory()->getOffsetPointer(0));
		size_t bufferSize = extent.height * stride;
		constexpr int depth = 24;  // TODO: Actually use window display depth.

		libXcb->xcb_put_image(
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

		libXcb->xcb_flush(connection);
	}

	return VK_SUCCESS;
}

}  // namespace vk