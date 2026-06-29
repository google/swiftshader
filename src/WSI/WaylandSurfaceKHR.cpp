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

#include "WaylandSurfaceKHR.hpp"

#include "libWaylandClient.hpp"
#include "Vulkan/VkDeviceMemory.hpp"
#include "Vulkan/VkImage.hpp"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace {

// Create an anonymous, in-memory file suitable for backing a wl_shm pool.
// Prefers memfd_create() (no filesystem footprint, no cleanup on crash) and
// falls back to an immediately-unlinked temp file on kernels without memfd.
int createAnonymousFile(off_t size)
{
	int fd = -1;

#if defined(SYS_memfd_create)
	fd = static_cast<int>(syscall(SYS_memfd_create, "swiftshader-wayland", 0 /* flags; MFD_CLOEXEC unavailable everywhere */));
	if(fd >= 0)
	{
		int flags = fcntl(fd, F_GETFD);
		if(flags != -1)
		{
			fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
		}
	}
#endif

	if(fd < 0)
	{
		char path[] = "/tmp/swiftshader-wayland-XXXXXX";
		fd = mkstemp(path);
		if(fd >= 0)
		{
			unlink(path);
		}
	}

	if(fd < 0)
	{
		return -1;
	}

	if(ftruncate(fd, size) < 0)
	{
		close(fd);
		return -1;
	}

	return fd;
}

// Maps the swapchain image format to a wl_shm pixel format. Only the formats
// advertised by SurfaceKHR::GetSurfaceFormats() (B8G8R8A8) need to be handled.
// VK_FORMAT_B8G8R8A8_* is a byte-order format (memory bytes B,G,R,A) and
// WL_SHM_FORMAT_XRGB8888 is the DRM fourcc XR24 with fixed memory bytes B,G,R,X;
// the two layouts coincide independently of host endianness, and the X/alpha
// byte is ignored as the surface only advertises VK_COMPOSITE_ALPHA_OPAQUE.
bool waylandShmFormat(VkFormat format, uint32_t *out)
{
	switch(format)
	{
	case VK_FORMAT_B8G8R8A8_UNORM:
	case VK_FORMAT_B8G8R8A8_SRGB:
		*out = WL_SHM_FORMAT_XRGB8888;
		return true;
	default:
		return false;
	}
}

}  // anonymous namespace

namespace vk {

bool WaylandSurfaceKHR::isSupported()
{
	return libWaylandClient.isPresent();
}

static void wl_registry_handle_global(void *data, struct wl_registry *registry, unsigned int name, const char *interface, unsigned int version)
{
	struct wl_shm **pshm = (struct wl_shm **)data;
	if(!strcmp(interface, "wl_shm"))
	{
		*pshm = static_cast<struct wl_shm *>(libWaylandClient->registry_bind(registry, name, libWaylandClient->wl_shm_interface, 1));
	}
}

static void wl_registry_handle_global_remove(void *data, struct wl_registry *registry, unsigned int name)
{
}

static const struct wl_registry_listener wl_registry_listener = { wl_registry_handle_global, wl_registry_handle_global_remove };

static void wl_buffer_handle_release(void *data, struct wl_buffer *buffer)
{
	static_cast<WaylandBuffer *>(data)->busy = false;
}

static const struct wl_buffer_listener wl_buffer_listener = { wl_buffer_handle_release };

WaylandSurfaceKHR::WaylandSurfaceKHR(const VkWaylandSurfaceCreateInfoKHR *pCreateInfo, void *mem)
    : display(pCreateInfo->display)
    , surface(pCreateInfo->surface)
{
	// Drive all WSI traffic on a private event queue so wl_display_dispatch /
	// roundtrip here never steal events belonging to the application's queue.
	queue = libWaylandClient->wl_display_create_queue(display);

	// Bind the registry (and everything created from it) to the private queue by
	// issuing get_registry on a queue-assigned proxy wrapper of the display.
	struct wl_display *displayWrapper = static_cast<struct wl_display *>(libWaylandClient->wl_proxy_create_wrapper(display));
	libWaylandClient->wl_proxy_set_queue(reinterpret_cast<wl_proxy *>(displayWrapper), queue);
	registry = libWaylandClient->display_get_registry(displayWrapper);
	libWaylandClient->wl_proxy_wrapper_destroy(displayWrapper);

	libWaylandClient->registry_add_listener(registry, &wl_registry_listener, &shm);
	// Roundtrip the private queue to receive the globals and bind wl_shm.
	libWaylandClient->wl_display_roundtrip_queue(display, queue);
}

void WaylandSurfaceKHR::destroySurface(const VkAllocationCallbacks *pAllocator)
{
	if(shm)
	{
		libWaylandClient->wl_proxy_destroy(reinterpret_cast<wl_proxy *>(shm));
		shm = nullptr;
	}
	if(registry)
	{
		libWaylandClient->wl_proxy_destroy(reinterpret_cast<wl_proxy *>(registry));
		registry = nullptr;
	}
	if(queue)
	{
		libWaylandClient->wl_event_queue_destroy(queue);
		queue = nullptr;
	}
}

size_t WaylandSurfaceKHR::ComputeRequiredAllocationSize(const VkWaylandSurfaceCreateInfoKHR *pCreateInfo)
{
	return 0;
}

VkResult WaylandSurfaceKHR::getSurfaceCapabilities(const void *pSurfaceInfoPNext, VkSurfaceCapabilitiesKHR *pSurfaceCapabilities, void *pSurfaceCapabilitiesPNext) const
{
	// The size of a Wayland surface is determined by the client (the swapchain
	// extent), so report the special "undefined" current extent.
	pSurfaceCapabilities->currentExtent = { 0xFFFFFFFF, 0xFFFFFFFF };
	pSurfaceCapabilities->minImageExtent = { 1, 1 };
	pSurfaceCapabilities->maxImageExtent = { 0xFFFFFFFF, 0xFFFFFFFF };

	SetCommonSurfaceCapabilities(pSurfaceInfoPNext, pSurfaceCapabilities, pSurfaceCapabilitiesPNext);
	return VK_SUCCESS;
}

bool WaylandSurfaceKHR::createBuffer(WaylandBuffer *out, const VkExtent3D &extent, int stride, uint32_t shmFormat)
{
	out->data = static_cast<uint8_t *>(MAP_FAILED);

	size_t size = static_cast<size_t>(extent.height) * stride;
	int fd = createAnonymousFile(size);
	if(fd < 0)
	{
		return false;
	}

	void *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(data == MAP_FAILED)
	{
		close(fd);
		return false;
	}

	struct wl_shm_pool *pool = libWaylandClient->shm_create_pool(shm, fd, static_cast<int32_t>(size));
	out->buffer = libWaylandClient->shm_pool_create_buffer(pool, 0, extent.width, extent.height, stride, shmFormat);
	libWaylandClient->shm_pool_destroy(pool);
	close(fd);

	if(!out->buffer)
	{
		munmap(data, size);
		out->data = static_cast<uint8_t *>(MAP_FAILED);
		return false;
	}

	out->data = static_cast<uint8_t *>(data);
	out->size = size;
	out->busy = false;
	// Release events are delivered on the private queue (inherited from wl_shm).
	libWaylandClient->buffer_add_listener(out->buffer, &wl_buffer_listener, out);
	return true;
}

void WaylandSurfaceKHR::attachImage(PresentImage *image)
{
	WaylandImage *wlImage = new WaylandImage;
	imageMap[image] = wlImage;

	uint32_t shmFormat;
	if(!shm || !waylandShmFormat(image->getImage()->getFormat(VK_IMAGE_ASPECT_COLOR_BIT), &shmFormat))
	{
		return;  // Leaves buffers unallocated; present() will fail gracefully.
	}

	const VkExtent3D &extent = image->getImage()->getExtent();
	int stride = image->getImage()->rowPitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, 0);

	for(WaylandBuffer &buffer : wlImage->buffers)
	{
		createBuffer(&buffer, extent, stride, shmFormat);
	}
}

void WaylandSurfaceKHR::detachImage(PresentImage *image)
{
	auto it = imageMap.find(image);
	if(it != imageMap.end())
	{
		WaylandImage *wlImage = it->second;
		for(WaylandBuffer &buffer : wlImage->buffers)
		{
			if(buffer.data != MAP_FAILED)
			{
				munmap(buffer.data, buffer.size);
			}
			if(buffer.buffer)
			{
				libWaylandClient->buffer_destroy(buffer.buffer);
			}
		}
		delete wlImage;
		imageMap.erase(it);
	}
}

VkResult WaylandSurfaceKHR::present(PresentImage *image)
{
	auto it = imageMap.find(image);
	if(it == imageMap.end())
	{
		return VK_SUCCESS;
	}

	WaylandImage *wlImage = it->second;

	// Alternate between the image's two buffers so we commit a different
	// wl_buffer than the one the compositor is currently showing; this releases
	// the previously-committed buffer and avoids overwriting a buffer in use.
	WaylandBuffer *buffer = &wlImage->buffers[wlImage->next];
	wlImage->next ^= 1;

	if(!buffer->buffer || buffer->data == MAP_FAILED)
	{
		return VK_ERROR_SURFACE_LOST_KHR;
	}

	// In the rare case the chosen buffer is still held by the compositor, wait
	// for its release before reusing it. This cannot deadlock: the other buffer
	// of the pair was committed since, so this one has been (or will be) released.
	while(buffer->busy)
	{
		if(libWaylandClient->wl_display_dispatch_queue(display, queue) < 0)
		{
			return VK_ERROR_SURFACE_LOST_KHR;
		}
	}

	const VkExtent3D &extent = image->getImage()->getExtent();
	int bufferRowPitch = image->getImage()->rowPitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, 0);
	image->getImage()->copyTo(buffer->data, bufferRowPitch);

	buffer->busy = true;
	libWaylandClient->surface_attach(surface, buffer->buffer, 0, 0);
	libWaylandClient->surface_damage(surface, 0, 0, extent.width, extent.height);
	libWaylandClient->surface_commit(surface);

	// EAGAIN merely means the compositor's socket buffer is full and not all data
	// was flushed; it is not a fatal error (the rest is sent on the next flush).
	if(libWaylandClient->wl_display_flush(display) < 0 && errno != EAGAIN)
	{
		return VK_ERROR_SURFACE_LOST_KHR;
	}

	return VK_SUCCESS;
}

}  // namespace vk
