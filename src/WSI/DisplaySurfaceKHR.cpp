// Copyright 2020 The SwiftShader Authors. All Rights Reserved.
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

#include "DisplaySurfaceKHR.hpp"

#include "Vulkan/VkDeviceMemory.hpp"
#include "Vulkan/VkImage.hpp"

#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <xf86drm.h>

namespace vk {

VkResult DisplaySurfaceKHR::GetDisplayModeProperties(uint32_t *pPropertyCount, VkDisplayModePropertiesKHR *pProperties)
{
	*pPropertyCount = 1;

	if(pProperties)
	{
		int fd = open("/dev/dri/card0", O_RDWR);
		drmModeRes *res = drmModeGetResources(fd);
		drmModeConnector *connector = drmModeGetConnector(fd, res->connectors[0]);
		pProperties->displayMode = (uintptr_t)connector->modes[0].name;
		pProperties->parameters.visibleRegion.width = connector->modes[0].hdisplay;
		pProperties->parameters.visibleRegion.height = connector->modes[0].vdisplay;
		pProperties->parameters.refreshRate = connector->modes[0].vrefresh * 1000;
		drmModeFreeConnector(connector);
		drmModeFreeResources(res);
		close(fd);
	}

	return VK_SUCCESS;
}

VkResult DisplaySurfaceKHR::GetDisplayPlaneCapabilities(VkDisplayPlaneCapabilitiesKHR *pCapabilities)
{
	int fd = open("/dev/dri/card0", O_RDWR);
	drmModeRes *res = drmModeGetResources(fd);
	drmModeConnector *connector = drmModeGetConnector(fd, res->connectors[0]);
	pCapabilities->supportedAlpha = VK_DISPLAY_PLANE_ALPHA_OPAQUE_BIT_KHR;
	pCapabilities->minSrcPosition.x = 0;
	pCapabilities->minSrcPosition.y = 0;
	pCapabilities->maxSrcPosition.x = 0;
	pCapabilities->maxSrcPosition.y = 0;
	pCapabilities->minSrcExtent.width = connector->modes[0].hdisplay;
	pCapabilities->minSrcExtent.height = connector->modes[0].vdisplay;
	pCapabilities->maxSrcExtent.width = connector->modes[0].hdisplay;
	pCapabilities->maxSrcExtent.height = connector->modes[0].vdisplay;
	pCapabilities->minDstPosition.x = 0;
	pCapabilities->minDstPosition.y = 0;
	pCapabilities->maxDstPosition.x = 0;
	pCapabilities->maxDstPosition.y = 0;
	pCapabilities->minDstExtent.width = connector->modes[0].hdisplay;
	pCapabilities->minDstExtent.height = connector->modes[0].vdisplay;
	pCapabilities->maxDstExtent.width = connector->modes[0].hdisplay;
	pCapabilities->maxDstExtent.height = connector->modes[0].vdisplay;
	drmModeFreeConnector(connector);
	drmModeFreeResources(res);
	close(fd);

	return VK_SUCCESS;
}

VkResult DisplaySurfaceKHR::GetDisplayPlaneSupportedDisplays(uint32_t *pDisplayCount, VkDisplayKHR *pDisplays)
{
	*pDisplayCount = 1;

	if(pDisplays)
	{
		int fd = open("/dev/dri/card0", O_RDWR);
		drmModeRes *res = drmModeGetResources(fd);
		*pDisplays = res->connectors[0];
		drmModeFreeResources(res);
		close(fd);
	}

	return VK_SUCCESS;
}

VkResult DisplaySurfaceKHR::GetPhysicalDeviceDisplayPlaneProperties(uint32_t *pPropertyCount, VkDisplayPlanePropertiesKHR *pProperties)
{
	*pPropertyCount = 1;

	if(pProperties)
	{
		int fd = open("/dev/dri/card0", O_RDWR);
		drmModeRes *res = drmModeGetResources(fd);
		pProperties->currentDisplay = res->connectors[0];
		pProperties->currentStackIndex = 0;
		drmModeFreeResources(res);
		close(fd);
	}

	return VK_SUCCESS;
}

VkResult DisplaySurfaceKHR::GetPhysicalDeviceDisplayProperties(uint32_t *pPropertyCount, VkDisplayPropertiesKHR *pProperties)
{
	*pPropertyCount = 1;

	if(pProperties)
	{
		int fd = open("/dev/dri/card0", O_RDWR);
		drmModeRes *res = drmModeGetResources(fd);
		drmModeConnector *connector = drmModeGetConnector(fd, res->connectors[0]);
		pProperties->display = res->connectors[0];
		pProperties->displayName = "monitor";
		pProperties->physicalDimensions.width = connector->mmWidth;
		pProperties->physicalDimensions.height = connector->mmHeight;
		if(pProperties->physicalDimensions.width <= 0 || pProperties->physicalDimensions.height <= 0)
		{
			pProperties->physicalDimensions.width = connector->modes[0].hdisplay * 25.4 / 96;
			pProperties->physicalDimensions.height = connector->modes[0].vdisplay * 25.4 / 96;
		}
		pProperties->physicalResolution.width = connector->modes[0].hdisplay;
		pProperties->physicalResolution.height = connector->modes[0].vdisplay;
		pProperties->supportedTransforms = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		pProperties->planeReorderPossible = VK_FALSE;
		pProperties->persistentContent = VK_FALSE;
		drmModeFreeConnector(connector);
		drmModeFreeResources(res);
		close(fd);
	}

	return VK_SUCCESS;
}

DisplaySurfaceKHR::DisplaySurfaceKHR(const VkDisplaySurfaceCreateInfoKHR *pCreateInfo, void *mem)
{
	fd = open("/dev/dri/card0", O_RDWR);
	drmModeRes *res = drmModeGetResources(fd);
	connector_id = res->connectors[0];
	drmModeFreeResources(res);
	drmModeConnector *connector = drmModeGetConnector(fd, connector_id);
	encoder_id = connector->encoder_id;
	memcpy(&mode_info, &connector->modes[0], sizeof(drmModeModeInfo));
	drmModeFreeConnector(connector);
	drmModeEncoder *encoder = drmModeGetEncoder(fd, encoder_id);
	crtc_id = encoder->crtc_id;
	drmModeFreeEncoder(encoder);

	crtc = drmModeGetCrtc(fd, crtc_id);

	struct drm_mode_create_dumb creq;
	memset(&creq, 0, sizeof(struct drm_mode_create_dumb));
	creq.width = mode_info.hdisplay;
	creq.height = mode_info.vdisplay;
	creq.bpp = 32;
	drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);

	handle = creq.handle;
	width = creq.width;
	height = creq.height;
	pitch = creq.pitch;
	size = creq.size;

	drmModeAddFB(fd, width, height, 24, 32, pitch, handle, &fb_id);

	struct drm_mode_map_dumb mreq;
	memset(&mreq, 0, sizeof(struct drm_mode_map_dumb));
	mreq.handle = handle;
	drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);

	fb_buffer = static_cast<uint8_t *>(mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, mreq.offset));
}

void DisplaySurfaceKHR::destroySurface(const VkAllocationCallbacks *pAllocator)
{
	munmap(fb_buffer, size);

	drmModeRmFB(fd, fb_id);

	struct drm_mode_destroy_dumb dreq;
	memset(&dreq, 0, sizeof(struct drm_mode_destroy_dumb));
	dreq.handle = handle;
	drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);

	drmModeSetCrtc(fd, crtc->crtc_id, crtc->buffer_id, crtc->x, crtc->y, &connector_id, 1, &crtc->mode);
	drmModeFreeCrtc(crtc);

	close(fd);
}

size_t DisplaySurfaceKHR::ComputeRequiredAllocationSize(const VkDisplaySurfaceCreateInfoKHR *pCreateInfo)
{
	return 0;
}

VkResult DisplaySurfaceKHR::getSurfaceCapabilities(VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) const
{
	setCommonSurfaceCapabilities(pSurfaceCapabilities);

	VkExtent2D extent = { width, height };

	pSurfaceCapabilities->currentExtent = extent;
	pSurfaceCapabilities->minImageExtent = extent;
	pSurfaceCapabilities->maxImageExtent = extent;
	return VK_SUCCESS;
}

void DisplaySurfaceKHR::attachImage(PresentImage *image)
{
}

void DisplaySurfaceKHR::detachImage(PresentImage *image)
{
}

VkResult DisplaySurfaceKHR::present(PresentImage *image)
{
	image->getImage()->copyTo(fb_buffer, pitch);
	drmModeSetCrtc(fd, crtc_id, fb_id, 0, 0, &connector_id, 1, &mode_info);

	return VK_SUCCESS;
}

}  // namespace vk
