// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
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

#ifndef GRALLOC_ANDROID
#define GRALLOC_ANDROID

#include <hardware/gralloc.h>
#include <hardware/gralloc1.h>

#ifdef HAVE_GRALLOC3
#	include <android/hardware/graphics/mapper/3.0/IMapper.h>
#	include <utils/StrongPointer.h>
#endif
#ifdef HAVE_GRALLOC4
#	include <android/hardware/graphics/mapper/4.0/IMapper.h>
#	include <utils/StrongPointer.h>
#endif

#include <unistd.h>  // for close()

class GrallocModule
{
public:
	static GrallocModule *getInstance();

	int import(buffer_handle_t handle, buffer_handle_t *imported_handle);
	int release(buffer_handle_t handle);

	int lock(buffer_handle_t handle, int usage, int left, int top, int width, int height, void **vaddr);
	int unlock(buffer_handle_t handle);

private:
	GrallocModule();
	uint8_t m_major_version;
	const gralloc_module_t *m_module;
#ifdef HAVE_GRALLOC1
	gralloc1_device_t *m_gralloc1_device = nullptr;
	GRALLOC1_PFN_LOCK m_gralloc1_lock = nullptr;
	GRALLOC1_PFN_UNLOCK m_gralloc1_unlock = nullptr;
#endif
#ifdef HAVE_GRALLOC3
	android::sp<android::hardware::graphics::mapper::V3_0::IMapper> m_gralloc3_mapper;
#endif
#ifdef HAVE_GRALLOC4
	android::sp<android::hardware::graphics::mapper::V4_0::IMapper> m_gralloc4_mapper;
#endif
};

#endif  // GRALLOC_ANDROID
