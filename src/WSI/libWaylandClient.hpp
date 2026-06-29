// Copyright 2022 The SwiftShader Authors. All Rights Reserved.
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

#ifndef libWaylandClient_hpp
#define libWaylandClient_hpp

#include <wayland-client.h>

// The Wayland protocol functions used by the WSI (wl_display_get_registry,
// wl_registry_bind, wl_surface_attach, wl_shm_create_pool, ...) are NOT real
// symbols exported by libwayland-client.so; they are `static inline` wrappers
// generated into <wayland-client-protocol.h> that marshal requests through the
// low-level wl_proxy_* primitives. SwiftShader loads libwayland-client.so
// dynamically (so it remains a self-contained ICD that also works where Wayland
// is absent), therefore it can only dlsym the real exported primitives and must
// re-implement those inline wrappers on top of them. dlsym'ing the wrapper names
// directly returns nullptr, so the surface previously crashed on first use (as
// seen when running ANGLE's EGLWaylandTest with SwiftShader).
struct LibWaylandClientExports
{
	LibWaylandClientExports() {}
	LibWaylandClientExports(void *libwl);

	// Real exported primitives: display and event-queue handling.
	int (*wl_display_dispatch_queue)(wl_display *display, wl_event_queue *queue) = nullptr;
	int (*wl_display_roundtrip_queue)(wl_display *display, wl_event_queue *queue) = nullptr;
	int (*wl_display_flush)(wl_display *display) = nullptr;
	wl_event_queue *(*wl_display_create_queue)(wl_display *display) = nullptr;
	void (*wl_event_queue_destroy)(wl_event_queue *queue) = nullptr;

	// Real exported primitives: proxy marshalling and lifetime.
	wl_proxy *(*wl_proxy_marshal_flags)(wl_proxy *proxy, uint32_t opcode, const wl_interface *interface, uint32_t version, uint32_t flags, ...) = nullptr;
	int (*wl_proxy_add_listener)(wl_proxy *proxy, void (**implementation)(void), void *data) = nullptr;
	void (*wl_proxy_destroy)(wl_proxy *proxy) = nullptr;
	uint32_t (*wl_proxy_get_version)(wl_proxy *proxy) = nullptr;
	void (*wl_proxy_set_queue)(wl_proxy *proxy, wl_event_queue *queue) = nullptr;
	void *(*wl_proxy_create_wrapper)(void *proxy) = nullptr;
	void (*wl_proxy_wrapper_destroy)(void *proxy_wrapper) = nullptr;

	// Real exported data symbols: protocol interface descriptions.
	const wl_interface *wl_registry_interface = nullptr;
	const wl_interface *wl_shm_interface = nullptr;
	const wl_interface *wl_shm_pool_interface = nullptr;
	const wl_interface *wl_surface_interface = nullptr;
	const wl_interface *wl_buffer_interface = nullptr;

	// Re-implementations of the generated static-inline protocol wrappers, built
	// on top of the loaded primitives above.
	wl_registry *display_get_registry(wl_display *display);
	int registry_add_listener(wl_registry *registry, const wl_registry_listener *listener, void *data);
	void *registry_bind(wl_registry *registry, uint32_t name, const wl_interface *interface, uint32_t version);
	wl_shm_pool *shm_create_pool(wl_shm *shm, int32_t fd, int32_t size);
	wl_buffer *shm_pool_create_buffer(wl_shm_pool *pool, int32_t offset, int32_t width, int32_t height, int32_t stride, uint32_t format);
	void shm_pool_destroy(wl_shm_pool *pool);
	int buffer_add_listener(wl_buffer *buffer, const wl_buffer_listener *listener, void *data);
	void buffer_destroy(wl_buffer *buffer);
	void surface_attach(wl_surface *surface, wl_buffer *buffer, int32_t x, int32_t y);
	void surface_damage(wl_surface *surface, int32_t x, int32_t y, int32_t width, int32_t height);
	void surface_commit(wl_surface *surface);

	// True when every primitive and interface required by the WSI resolved.
	bool loaded() const;
};

class LibWaylandClient
{
public:
	bool isPresent()
	{
		return loadExports() != nullptr;
	}

	LibWaylandClientExports *operator->();

private:
	LibWaylandClientExports *loadExports();
};

extern LibWaylandClient libWaylandClient;

#endif  // libWaylandClient_hpp
