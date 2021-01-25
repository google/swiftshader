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

#include "VulkanBenchmark.hpp"

VulkanBenchmark::~VulkanBenchmark()
{
	device.waitIdle();
	device.destroy(nullptr);
	instance.destroy(nullptr);
}

void VulkanBenchmark::initialize()
{
	// TODO(b/158231104): Other platforms
#if defined(_WIN32)
	dl = std::make_unique<vk::DynamicLoader>("./vk_swiftshader.dll");
#elif defined(__linux__)
	dl = std::make_unique<vk::DynamicLoader>("./libvk_swiftshader.so");
#else
#	error Unimplemented platform
#endif
	assert(dl->success());

	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl->getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

	instance = vk::createInstance({}, nullptr);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

	std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
	assert(!physicalDevices.empty());
	physicalDevice = physicalDevices[0];

	const float defaultQueuePriority = 0.0f;
	vk::DeviceQueueCreateInfo queueCreatInfo;
	queueCreatInfo.queueFamilyIndex = queueFamilyIndex;
	queueCreatInfo.queueCount = 1;
	queueCreatInfo.pQueuePriorities = &defaultQueuePriority;

	vk::DeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &queueCreatInfo;

	device = physicalDevice.createDevice(deviceCreateInfo, nullptr);

	queue = device.getQueue(queueFamilyIndex, 0);
}
