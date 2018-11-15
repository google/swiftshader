// Copyright 2018 The SwiftShader Authors. All Rights Reserved.
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

#ifndef VK_SHADER_MODULE_HPP_
#define VK_SHADER_MODULE_HPP_

#include "VkObject.hpp"

namespace vk
{

class ShaderModule : public Object<ShaderModule, VkShaderModule>
{
public:
	ShaderModule(const VkShaderModuleCreateInfo* pCreateInfo, void* mem);
	~ShaderModule() = delete;
	void destroy(const VkAllocationCallbacks* pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkShaderModuleCreateInfo* pCreateInfo);

private:
	uint32_t* code = nullptr;
};

static inline ShaderModule* Cast(VkShaderModule object)
{
	return reinterpret_cast<ShaderModule*>(object);
}

} // namespace vk

#endif // VK_SHADER_MODULE_HPP_
