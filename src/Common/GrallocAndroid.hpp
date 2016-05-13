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

class GrallocModule
{
public:
	static GrallocModule *getInstance();
	int lock(buffer_handle_t handle, int usage, int left, int top, int width, int height, void **vaddr)
	{
		return m_module->lock(m_module, handle, usage, left, top, width, height, vaddr);
	}

	int unlock(buffer_handle_t handle)
	{
		return m_module->unlock(m_module, handle);
	}

private:
	GrallocModule();
	const gralloc_module_t *m_module;
};

#endif  // GRALLOC_ANDROID
