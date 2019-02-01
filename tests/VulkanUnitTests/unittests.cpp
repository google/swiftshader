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

#include "Driver.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <cstring>

class SwiftShaderVulkanTest : public testing::Test
{
};

TEST_F(SwiftShaderVulkanTest, ICD_Check)
{
    Driver driver;
    ASSERT_TRUE(driver.loadSwiftShader());

    auto createInstance = driver.vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, "vkCreateInstance");
    EXPECT_NE(createInstance, nullptr);

    auto enumerateInstanceExtensionProperties =
        driver.vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceExtensionProperties");
    EXPECT_NE(enumerateInstanceExtensionProperties, nullptr);

    auto enumerateInstanceLayerProperties =
        driver.vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceLayerProperties");
    EXPECT_NE(enumerateInstanceLayerProperties, nullptr);

    auto enumerateInstanceVersion = driver.vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion");
    EXPECT_NE(enumerateInstanceVersion, nullptr);

    auto bad_function = driver.vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, "bad_function");
    EXPECT_EQ(bad_function, nullptr);
}

TEST_F(SwiftShaderVulkanTest, Version)
{
    Driver driver;
    ASSERT_TRUE(driver.loadSwiftShader());

    uint32_t apiVersion = 0;
    VkResult result = driver.vkEnumerateInstanceVersion(&apiVersion);
    EXPECT_EQ(apiVersion, (uint32_t)VK_API_VERSION_1_1);

    const VkInstanceCreateInfo createInfo = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,  // sType
        nullptr,                                 // pNext
        0,                                       // flags
        nullptr,                                 // pApplicationInfo
        0,                                       // enabledLayerCount
        nullptr,                                 // ppEnabledLayerNames
        0,                                       // enabledExtensionCount
        nullptr,                                 // ppEnabledExtensionNames
    };
    VkInstance instance = VK_NULL_HANDLE;
    result = driver.vkCreateInstance(&createInfo, nullptr, &instance);
    EXPECT_EQ(result, VK_SUCCESS);

    ASSERT_TRUE(driver.resolve(instance));

    uint32_t pPhysicalDeviceCount = 0;
    result = driver.vkEnumeratePhysicalDevices(instance, &pPhysicalDeviceCount, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(pPhysicalDeviceCount, 1U);

    VkPhysicalDevice pPhysicalDevice = VK_NULL_HANDLE;
    result = driver.vkEnumeratePhysicalDevices(instance, &pPhysicalDeviceCount, &pPhysicalDevice);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_NE(pPhysicalDevice, (VkPhysicalDevice)VK_NULL_HANDLE);

    VkPhysicalDeviceProperties physicalDeviceProperties;
    driver.vkGetPhysicalDeviceProperties(pPhysicalDevice, &physicalDeviceProperties);
    EXPECT_EQ(physicalDeviceProperties.apiVersion, (uint32_t)VK_API_VERSION_1_1);
    EXPECT_EQ(physicalDeviceProperties.deviceID, 0xC0DEU);
    EXPECT_EQ(physicalDeviceProperties.deviceType, VK_PHYSICAL_DEVICE_TYPE_CPU);

    EXPECT_EQ(strncmp(physicalDeviceProperties.deviceName, "SwiftShader Device", VK_MAX_PHYSICAL_DEVICE_NAME_SIZE), 0);
}
