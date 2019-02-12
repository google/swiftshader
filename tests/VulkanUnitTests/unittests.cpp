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

// Vulkan unit tests that provide coverage for functionality not tested by
// the dEQP test suite. Also used as a smoke test.

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vulkan/vulkan_core.h>
#include <vulkan/vk_icd.h>

#include <cstring>

typedef PFN_vkVoidFunction(__stdcall *vk_icdGetInstanceProcAddrPtr)(VkInstance, const char*);

#if defined(_WIN32)
#include <Windows.h>
#endif

class SwiftShaderVulkanTest : public testing::Test
{
protected:
	void SetUp() override
	{
		HMODULE libVulkan = nullptr;
		const char* libVulkanName = nullptr;

		#if defined(_WIN64)
			#if defined(NDEBUG)
				libVulkanName = "../../out/Release_x64/vk_swiftshader.dll";
			#else
				libVulkanName = "../../out/Debug_x64/vk_swiftshader.dll";
			#endif
		#else
			#error Unimplemented platform
		#endif

		#if defined(_WIN32)
			libVulkan = LoadLibraryA(libVulkanName);
			EXPECT_NE((HMODULE)NULL, libVulkan);
			vk_icdGetInstanceProcAddr = (vk_icdGetInstanceProcAddrPtr)GetProcAddress(libVulkan, "vk_icdGetInstanceProcAddr");
			EXPECT_NE((vk_icdGetInstanceProcAddrPtr)nullptr, vk_icdGetInstanceProcAddr);
		#endif
	}

	vk_icdGetInstanceProcAddrPtr vk_icdGetInstanceProcAddr = nullptr;
};

TEST_F(SwiftShaderVulkanTest, ICD_Check)
{
	if(vk_icdGetInstanceProcAddr)
	{
		auto createInstance = vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, "vkCreateInstance");
		EXPECT_NE(createInstance, nullptr);

		auto enumerateInstanceExtensionProperties = vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceExtensionProperties");
		EXPECT_NE(enumerateInstanceExtensionProperties, nullptr);

		auto enumerateInstanceLayerProperties = vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceLayerProperties");
		EXPECT_NE(enumerateInstanceLayerProperties, nullptr);

		auto enumerateInstanceVersion = vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion");
		EXPECT_NE(enumerateInstanceVersion, nullptr);

		auto bad_function = vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, "bad_function");
		EXPECT_EQ(bad_function, nullptr);
	}
}

TEST_F(SwiftShaderVulkanTest, Version)
{
	uint32_t apiVersion = 0;
	VkResult result = vkEnumerateInstanceVersion(&apiVersion);
	EXPECT_EQ(apiVersion, VK_API_VERSION_1_1);

	const VkInstanceCreateInfo createInfo =
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // sType
		nullptr, // pNext
		0,       // flags
		nullptr, // pApplicationInfo
		0,       // enabledLayerCount
		nullptr, // ppEnabledLayerNames
		0,       // enabledExtensionCount
		nullptr, // ppEnabledExtensionNames
	};
	VkInstance instance = VK_NULL_HANDLE;
	result = vkCreateInstance(&createInfo, nullptr, &instance);
	EXPECT_EQ(result, VK_SUCCESS);

	uint32_t pPhysicalDeviceCount = 0;
	result = vkEnumeratePhysicalDevices(instance, &pPhysicalDeviceCount, nullptr);
	EXPECT_EQ(result, VK_SUCCESS);
	EXPECT_EQ(pPhysicalDeviceCount, 1);

	VkPhysicalDevice pPhysicalDevice = VK_NULL_HANDLE;
	result = vkEnumeratePhysicalDevices(instance, &pPhysicalDeviceCount, &pPhysicalDevice);
	EXPECT_EQ(result, VK_SUCCESS);
	EXPECT_NE(pPhysicalDevice, (VkPhysicalDevice)VK_NULL_HANDLE);

	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(pPhysicalDevice, &physicalDeviceProperties);
	EXPECT_EQ(physicalDeviceProperties.apiVersion, VK_API_VERSION_1_1);
	EXPECT_EQ(physicalDeviceProperties.deviceID, 0xC0DE);
	EXPECT_EQ(physicalDeviceProperties.deviceType, VK_PHYSICAL_DEVICE_TYPE_CPU);

	EXPECT_EQ(strncmp(physicalDeviceProperties.deviceName, "SwiftShader Device", VK_MAX_PHYSICAL_DEVICE_NAME_SIZE), 0);
}
