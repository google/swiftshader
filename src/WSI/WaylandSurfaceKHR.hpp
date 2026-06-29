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

#ifndef SWIFTSHADER_WAYLANDSURFACEKHR_HPP
#define SWIFTSHADER_WAYLANDSURFACEKHR_HPP

#include "VkSurfaceKHR.hpp"
#include "Vulkan/VkObject.hpp"

#include <vulkan/vulkan_wayland.h>
#include <wayland-client.h>

#include <unordered_map>

namespace vk {

struct WaylandBuffer
{
	struct wl_buffer *buffer = nullptr;
	uint8_t *data = nullptr;  // mmap'd shm pointer, or MAP_FAILED on failure
	size_t size = 0;          // length of the mmap'd region, for munmap
	bool busy = false;        // true while the compositor still holds this buffer
};

struct WaylandImage
{
	// Two buffers per presentable image so a present can always commit a buffer
	// the compositor isn't reading. SwiftShader's swapchain marks an image
	// AVAILABLE as soon as it is presented, so a client may re-present the same
	// image every frame; committing the same wl_buffer each time would deadlock
	// (the compositor only releases a buffer once a different one is committed)
	// and overwriting it would tear.
	WaylandBuffer buffers[2];
	int next = 0;
};

class WaylandSurfaceKHR : public SurfaceKHR, public ObjectBase<WaylandSurfaceKHR, VkSurfaceKHR>
{
public:
	static bool isSupported();
	WaylandSurfaceKHR(const VkWaylandSurfaceCreateInfoKHR *pCreateInfo, void *mem);

	void destroySurface(const VkAllocationCallbacks *pAllocator) override;

	static size_t ComputeRequiredAllocationSize(const VkWaylandSurfaceCreateInfoKHR *pCreateInfo);

	VkResult getSurfaceCapabilities(const void *pSurfaceInfoPNext, VkSurfaceCapabilitiesKHR *pSurfaceCapabilities, void *pSurfaceCapabilitiesPNext) const override;

	virtual void attachImage(PresentImage *image) override;
	virtual void detachImage(PresentImage *image) override;
	VkResult present(PresentImage *image) override;

private:
	struct wl_display *display = nullptr;
	struct wl_surface *surface = nullptr;
	// A private event queue keeps the roundtrips/dispatches done by the WSI from
	// consuming events that belong to the application's own event loop on the
	// shared wl_display.
	struct wl_event_queue *queue = nullptr;
	struct wl_registry *registry = nullptr;
	struct wl_shm *shm = nullptr;
	std::unordered_map<PresentImage *, WaylandImage *> imageMap;

	bool createBuffer(WaylandBuffer *out, const VkExtent3D &extent, int stride, uint32_t shmFormat);
};

}  // namespace vk
#endif  // SWIFTSHADER_WAYLANDSURFACEKHR_HPP
