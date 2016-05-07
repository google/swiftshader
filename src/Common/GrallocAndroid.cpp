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

#include "GrallocAndroid.hpp"

#include <cutils/log.h>

GrallocModule *GrallocModule::getInstance()
{
	static GrallocModule instance;
	return &instance;
}

GrallocModule::GrallocModule()
{
	const hw_module_t *module = nullptr;
	hw_get_module(GRALLOC_HARDWARE_MODULE_ID, &module);

	if(!module)
	{
		ALOGE("Failed to load standard gralloc");
	}

	m_module = reinterpret_cast<const gralloc_module_t*>(module);
}
