// Copyright 2021 The SwiftShader Authors. All Rights Reserved.
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

#include "libWaylandClient.hpp"

#include "System/SharedLibrary.hpp"

#include <memory>

LibWaylandClientExports::LibWaylandClientExports(void *libwl)
{
	getFuncAddress(libwl, "wl_display_dispatch_queue", &wl_display_dispatch_queue);
	getFuncAddress(libwl, "wl_display_roundtrip_queue", &wl_display_roundtrip_queue);
	getFuncAddress(libwl, "wl_display_flush", &wl_display_flush);
	getFuncAddress(libwl, "wl_display_create_queue", &wl_display_create_queue);
	getFuncAddress(libwl, "wl_event_queue_destroy", &wl_event_queue_destroy);

	getFuncAddress(libwl, "wl_proxy_marshal_flags", &wl_proxy_marshal_flags);
	getFuncAddress(libwl, "wl_proxy_add_listener", &wl_proxy_add_listener);
	getFuncAddress(libwl, "wl_proxy_destroy", &wl_proxy_destroy);
	getFuncAddress(libwl, "wl_proxy_get_version", &wl_proxy_get_version);
	getFuncAddress(libwl, "wl_proxy_set_queue", &wl_proxy_set_queue);
	getFuncAddress(libwl, "wl_proxy_create_wrapper", &wl_proxy_create_wrapper);
	getFuncAddress(libwl, "wl_proxy_wrapper_destroy", &wl_proxy_wrapper_destroy);

	wl_registry_interface = reinterpret_cast<const wl_interface *>(getProcAddress(libwl, "wl_registry_interface"));
	wl_shm_interface = reinterpret_cast<const wl_interface *>(getProcAddress(libwl, "wl_shm_interface"));
	wl_shm_pool_interface = reinterpret_cast<const wl_interface *>(getProcAddress(libwl, "wl_shm_pool_interface"));
	wl_surface_interface = reinterpret_cast<const wl_interface *>(getProcAddress(libwl, "wl_surface_interface"));
	wl_buffer_interface = reinterpret_cast<const wl_interface *>(getProcAddress(libwl, "wl_buffer_interface"));
}

bool LibWaylandClientExports::loaded() const
{
	return wl_display_dispatch_queue && wl_display_roundtrip_queue && wl_display_flush &&
	       wl_display_create_queue && wl_event_queue_destroy &&
	       wl_proxy_marshal_flags && wl_proxy_add_listener && wl_proxy_destroy &&
	       wl_proxy_get_version && wl_proxy_set_queue && wl_proxy_create_wrapper &&
	       wl_proxy_wrapper_destroy &&
	       wl_registry_interface && wl_shm_interface && wl_shm_pool_interface &&
	       wl_surface_interface && wl_buffer_interface;
}

// The following re-implement the static-inline wrappers from
// <wayland-client-protocol.h>, mirroring exactly the wl_proxy_marshal_flags()
// calls they generate (opcodes and WL_MARSHAL_FLAG_DESTROY come from that
// header). They marshal through the dynamically-loaded primitives instead of
// referencing libwayland-client symbols at link time.

wl_registry *LibWaylandClientExports::display_get_registry(wl_display *display)
{
	wl_proxy *proxy = reinterpret_cast<wl_proxy *>(display);
	return reinterpret_cast<wl_registry *>(wl_proxy_marshal_flags(
	    proxy, WL_DISPLAY_GET_REGISTRY, wl_registry_interface,
	    wl_proxy_get_version(proxy), 0, static_cast<void *>(nullptr)));
}

int LibWaylandClientExports::registry_add_listener(wl_registry *registry, const wl_registry_listener *listener, void *data)
{
	return wl_proxy_add_listener(reinterpret_cast<wl_proxy *>(registry),
	                             reinterpret_cast<void (**)(void)>(const_cast<wl_registry_listener *>(listener)), data);
}

void *LibWaylandClientExports::registry_bind(wl_registry *registry, uint32_t name, const wl_interface *interface, uint32_t version)
{
	wl_proxy *proxy = reinterpret_cast<wl_proxy *>(registry);
	return reinterpret_cast<void *>(wl_proxy_marshal_flags(
	    proxy, WL_REGISTRY_BIND, interface, version, 0,
	    name, interface->name, version, static_cast<void *>(nullptr)));
}

wl_shm_pool *LibWaylandClientExports::shm_create_pool(wl_shm *shm, int32_t fd, int32_t size)
{
	wl_proxy *proxy = reinterpret_cast<wl_proxy *>(shm);
	return reinterpret_cast<wl_shm_pool *>(wl_proxy_marshal_flags(
	    proxy, WL_SHM_CREATE_POOL, wl_shm_pool_interface,
	    wl_proxy_get_version(proxy), 0, static_cast<void *>(nullptr), fd, size));
}

wl_buffer *LibWaylandClientExports::shm_pool_create_buffer(wl_shm_pool *pool, int32_t offset, int32_t width, int32_t height, int32_t stride, uint32_t format)
{
	wl_proxy *proxy = reinterpret_cast<wl_proxy *>(pool);
	return reinterpret_cast<wl_buffer *>(wl_proxy_marshal_flags(
	    proxy, WL_SHM_POOL_CREATE_BUFFER, wl_buffer_interface,
	    wl_proxy_get_version(proxy), 0, static_cast<void *>(nullptr), offset, width, height, stride, format));
}

void LibWaylandClientExports::shm_pool_destroy(wl_shm_pool *pool)
{
	wl_proxy *proxy = reinterpret_cast<wl_proxy *>(pool);
	wl_proxy_marshal_flags(proxy, WL_SHM_POOL_DESTROY, nullptr,
	                       wl_proxy_get_version(proxy), WL_MARSHAL_FLAG_DESTROY);
}

int LibWaylandClientExports::buffer_add_listener(wl_buffer *buffer, const wl_buffer_listener *listener, void *data)
{
	return wl_proxy_add_listener(reinterpret_cast<wl_proxy *>(buffer),
	                             reinterpret_cast<void (**)(void)>(const_cast<wl_buffer_listener *>(listener)), data);
}

void LibWaylandClientExports::buffer_destroy(wl_buffer *buffer)
{
	wl_proxy *proxy = reinterpret_cast<wl_proxy *>(buffer);
	wl_proxy_marshal_flags(proxy, WL_BUFFER_DESTROY, nullptr,
	                       wl_proxy_get_version(proxy), WL_MARSHAL_FLAG_DESTROY);
}

void LibWaylandClientExports::surface_attach(wl_surface *surface, wl_buffer *buffer, int32_t x, int32_t y)
{
	wl_proxy *proxy = reinterpret_cast<wl_proxy *>(surface);
	wl_proxy_marshal_flags(proxy, WL_SURFACE_ATTACH, nullptr,
	                       wl_proxy_get_version(proxy), 0, buffer, x, y);
}

void LibWaylandClientExports::surface_damage(wl_surface *surface, int32_t x, int32_t y, int32_t width, int32_t height)
{
	wl_proxy *proxy = reinterpret_cast<wl_proxy *>(surface);
	wl_proxy_marshal_flags(proxy, WL_SURFACE_DAMAGE, nullptr,
	                       wl_proxy_get_version(proxy), 0, x, y, width, height);
}

void LibWaylandClientExports::surface_commit(wl_surface *surface)
{
	wl_proxy *proxy = reinterpret_cast<wl_proxy *>(surface);
	wl_proxy_marshal_flags(proxy, WL_SURFACE_COMMIT, nullptr,
	                       wl_proxy_get_version(proxy), 0);
}

LibWaylandClientExports *LibWaylandClient::operator->()
{
	return loadExports();
}

LibWaylandClientExports *LibWaylandClient::loadExports()
{
	static LibWaylandClientExports exports = [] {
		void *libwl = nullptr;

		if(getProcAddress(RTLD_DEFAULT, "wl_proxy_marshal_flags"))  // Search the global scope for pre-loaded Wayland client library.
		{
			libwl = RTLD_DEFAULT;
		}
		else
		{
			libwl = loadLibrary("libwayland-client.so.0");
		}

		return LibWaylandClientExports(libwl);
	}();

	return exports.loaded() ? &exports : nullptr;
}

LibWaylandClient libWaylandClient;
