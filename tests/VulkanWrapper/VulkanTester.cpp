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

#include "VulkanTester.hpp"
#include <fstream>

#if defined(_WIN32)
#	define OS_WINDOWS 1
#elif defined(__APPLE__)
#	include "dlfcn.h"
#	define OS_MAC 1
#elif defined(__ANDROID__)
#	include "dlfcn.h"
#	define OS_ANDROID 1
#elif defined(__linux__)
#	include "dlfcn.h"
#	define OS_LINUX 1
#elif defined(__Fuchsia__)
#	include <zircon/dlfcn.h>
#	define OS_FUCHSIA 1
#else
#	error Unimplemented platform
#endif

namespace {
std::vector<const char *> getDriverPaths()
{
#if OS_WINDOWS
#	if !defined(STANDALONE)
	// The DLL is delay loaded (see BUILD.gn), so we can load
	// the correct ones from Chrome's swiftshader subdirectory.
	// HMODULE libvulkan = LoadLibraryA("swiftshader\\libvulkan.dll");
	// EXPECT_NE((HMODULE)NULL, libvulkan);
	// return true;
#		error TODO: !STANDALONE
#	elif defined(NDEBUG)
#		if defined(_WIN64)
	return { "./build/Release_x64/vk_swiftshader.dll",
		     "./build/Release/vk_swiftshader.dll",
		     "./vk_swiftshader.dll" };
#		else
	return { "./build/Release_Win32/vk_swiftshader.dll",
		     "./build/Release/vk_swiftshader.dll",
		     "./vk_swiftshader.dll" };
#		endif
#	else
#		if defined(_WIN64)
	return { "./build/Debug_x64/vk_swiftshader.dll",
		     "./build/Debug/vk_swiftshader.dll",
		     "./vk_swiftshader.dll" };
#		else
	return { "./build/Debug_Win32/vk_swiftshader.dll",
		     "./build/Debug/vk_swiftshader.dll",
		     "./vk_swiftshader.dll" };
#		endif
#	endif
#elif OS_MAC
	return { "./build/Darwin/libvk_swiftshader.dylib",
		     "swiftshader/libvk_swiftshader.dylib",
		     "libvk_swiftshader.dylib" };
#elif OS_LINUX
	return { "./build/Linux/libvk_swiftshader.so",
		     "swiftshader/libvk_swiftshader.so",
		     "./libvk_swiftshader.so",
		     "libvk_swiftshader.so" };
#elif OS_ANDROID || OS_FUCHSIA
	return
	{
		"libvk_swiftshader.so"
	}
#else
#	error Unimplemented platform
	return {};
#endif
}

bool fileExists(const char *path)
{
	std::ifstream f(path);
	return f.good();
}

std::unique_ptr<vk::DynamicLoader> loadDriver()
{
	for(auto &p : getDriverPaths())
	{
		if(!fileExists(p))
			continue;
		return std::make_unique<vk::DynamicLoader>(p);
	}

#if(OS_MAC || OS_LINUX || OS_ANDROID || OS_FUCHSIA)
	// On Linux-based OSes, the lib path may be resolved by dlopen
	for(auto &p : getDriverPaths())
	{
		auto lib = dlopen(p, RTLD_LAZY | RTLD_LOCAL);
		if(lib)
		{
			dlclose(lib);
			return std::make_unique<vk::DynamicLoader>(p);
		}
	}
#endif

	return {};
}

}  // namespace

VulkanTester::~VulkanTester()
{
	device.waitIdle();
	device.destroy(nullptr);
	instance.destroy(nullptr);
}

void VulkanTester::initialize()
{
	dl = loadDriver();
	assert(dl && dl->success());

	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl->getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

	instance = vk::createInstance({}, nullptr);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

	std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
	assert(!physicalDevices.empty());
	physicalDevice = physicalDevices[0];

	const float defaultQueuePriority = 0.0f;
	vk::DeviceQueueCreateInfo queueCreateInfo;
	queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &defaultQueuePriority;

	vk::DeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;

	device = physicalDevice.createDevice(deviceCreateInfo, nullptr);

	queue = device.getQueue(queueFamilyIndex, 0);
}
