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

#ifndef SWIFTSHADER_XLIBSURFACEKHR_HPP
#define SWIFTSHADER_XLIBSURFACEKHR_HPP

#include "Vulkan/VkObject.hpp"
#include "libX11.hpp"
#include "VkSurfaceKHR.hpp"

namespace vk {

class XlibSurfaceKHR : public SurfaceKHR, public ObjectBase<XlibSurfaceKHR, VkSurfaceKHR> {
public:
	XlibSurfaceKHR(const VkXlibSurfaceCreateInfoKHR *pCreateInfo, void *mem);

	~XlibSurfaceKHR() = delete;

	void destroySurface(const VkAllocationCallbacks *pAllocator) override;

	static size_t ComputeRequiredAllocationSize(const VkXlibSurfaceCreateInfoKHR *pCreateInfo);

	void getSurfaceCapabilities(VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) const override;

private:
	Display *pDisplay;
	Window window;

};

}
#endif //SWIFTSHADER_XLIBSURFACEKHR_HPP
