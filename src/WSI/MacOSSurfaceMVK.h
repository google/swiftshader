// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
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

#ifndef SWIFTSHADER_MACOSSURFACEMVK_HPP
#define SWIFTSHADER_MACOSSURFACEMVK_HPP

#include "Vulkan/VkObject.hpp"
#include "VkSurfaceKHR.hpp"
#include "vulkan/vulkan_macos.h"

namespace vk {

class MetalLayer;

class MacOSSurfaceMVK : public SurfaceKHR, public ObjectBase<MacOSSurfaceMVK, VkSurfaceKHR> {
public:
    MacOSSurfaceMVK(const VkMacOSSurfaceCreateInfoMVK *pCreateInfo, void *mem);

    void destroySurface(const VkAllocationCallbacks *pAllocator) override;

    static size_t ComputeRequiredAllocationSize(const VkMacOSSurfaceCreateInfoMVK *pCreateInfo);

    void getSurfaceCapabilities(VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) const override;

    virtual void attachImage(PresentImage* image) override {}
    virtual void detachImage(PresentImage* image) override {}
    void present(PresentImage* image) override;

private:
    MetalLayer* metalLayer = nullptr;
};

}
#endif //SWIFTSHADER_MACOSSURFACEMVK_HPP
