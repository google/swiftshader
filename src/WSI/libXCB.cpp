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

#include "libXCB.hpp"

#include "System/SharedLibrary.hpp"

#include <memory>

LibXcbExports::LibXcbExports(void *lib)
{
	getFuncAddress(lib, "xcb_create_gc", &xcb_create_gc);
	getFuncAddress(lib, "xcb_flush", &xcb_flush);
	getFuncAddress(lib, "xcb_free_gc", &xcb_free_gc);
	getFuncAddress(lib, "xcb_generate_id", &xcb_generate_id);
	getFuncAddress(lib, "xcb_get_geometry", &xcb_get_geometry);
	getFuncAddress(lib, "xcb_get_geometry_reply", &xcb_get_geometry_reply);
	getFuncAddress(lib, "xcb_put_image", &xcb_put_image);
}

LibXcbExports *LibXCB::operator->()
{
	return loadExports();
}

LibXcbExports *LibXCB::loadExports()
{
	static LibXcbExports exports = [] {
		if(getProcAddress(RTLD_DEFAULT, "xcb_create_gc"))  // Search the global scope for pre-loaded XCB library.
		{
			return LibXcbExports(RTLD_DEFAULT);
		}

		if(void *lib = loadLibrary("libxcb.so.1"))
		{
			return LibXcbExports(lib);
		}

		return LibXcbExports();
	}();

	return exports.xcb_create_gc ? &exports : nullptr;
}

LibXCB libXCB;
