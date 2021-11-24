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

#ifndef libXCB_hpp
#define libXCB_hpp

#include <xcb/xcb.h>

struct LibXcbExports
{
	LibXcbExports() {}
	LibXcbExports(void *lib);

	xcb_void_cookie_t (*xcb_create_gc)(xcb_connection_t *c, xcb_gcontext_t cid, xcb_drawable_t drawable, uint32_t value_mask, const void *value_list) = nullptr;
	int (*xcb_flush)(xcb_connection_t *c) = nullptr;
	xcb_void_cookie_t (*xcb_free_gc)(xcb_connection_t *c, xcb_gcontext_t gc) = nullptr;
	uint32_t (*xcb_generate_id)(xcb_connection_t *c) = nullptr;
	xcb_get_geometry_cookie_t (*xcb_get_geometry)(xcb_connection_t *c, xcb_drawable_t drawable) = nullptr;
	xcb_get_geometry_reply_t *(*xcb_get_geometry_reply)(xcb_connection_t *c, xcb_get_geometry_cookie_t cookie, xcb_generic_error_t **e) = nullptr;
	xcb_void_cookie_t (*xcb_put_image)(xcb_connection_t *c, uint8_t format, xcb_drawable_t drawable, xcb_gcontext_t gc, uint16_t width, uint16_t height, int16_t dst_x, int16_t dst_y, uint8_t left_pad, uint8_t depth, uint32_t data_len, const uint8_t *data) = nullptr;
};

class LibXCB
{
public:
	bool isPresent()
	{
		return loadExports() != nullptr;
	}

	LibXcbExports *operator->();

private:
	LibXcbExports *loadExports();
};

extern LibXCB libXCB;

#endif  // libXCB_hpp
