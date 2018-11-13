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

#include <vulkan/vulkan.h>
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

TEST_F(SwiftShaderVulkanTest, API_Check)
{
	if(vk_icdGetInstanceProcAddr)
	{
		#define API_CHECK(function) auto function = vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, #function); \
		                            EXPECT_NE(function, nullptr);

		API_CHECK(vkCreateInstance);
		API_CHECK(vkDestroyInstance);
		API_CHECK(vkEnumeratePhysicalDevices);
		API_CHECK(vkGetPhysicalDeviceFeatures);
		API_CHECK(vkGetPhysicalDeviceFormatProperties);
		API_CHECK(vkGetPhysicalDeviceImageFormatProperties);
		API_CHECK(vkGetPhysicalDeviceProperties);
		API_CHECK(vkGetPhysicalDeviceQueueFamilyProperties);
		API_CHECK(vkGetPhysicalDeviceMemoryProperties);
		API_CHECK(vkGetInstanceProcAddr);
		API_CHECK(vkGetDeviceProcAddr);
		API_CHECK(vkCreateDevice);
		API_CHECK(vkDestroyDevice);
		API_CHECK(vkEnumerateInstanceExtensionProperties);
		API_CHECK(vkEnumerateDeviceExtensionProperties);
		API_CHECK(vkEnumerateInstanceLayerProperties);
		API_CHECK(vkEnumerateDeviceLayerProperties);
		API_CHECK(vkGetDeviceQueue);
		API_CHECK(vkQueueSubmit);
		API_CHECK(vkQueueWaitIdle);
		API_CHECK(vkDeviceWaitIdle);
		API_CHECK(vkAllocateMemory);
		API_CHECK(vkFreeMemory);
		API_CHECK(vkMapMemory);
		API_CHECK(vkUnmapMemory);
		API_CHECK(vkFlushMappedMemoryRanges);
		API_CHECK(vkInvalidateMappedMemoryRanges);
		API_CHECK(vkGetDeviceMemoryCommitment);
		API_CHECK(vkBindBufferMemory);
		API_CHECK(vkBindImageMemory);
		API_CHECK(vkGetBufferMemoryRequirements);
		API_CHECK(vkGetImageMemoryRequirements);
		API_CHECK(vkGetImageSparseMemoryRequirements);
		API_CHECK(vkGetPhysicalDeviceSparseImageFormatProperties);
		API_CHECK(vkQueueBindSparse);
		API_CHECK(vkCreateFence);
		API_CHECK(vkDestroyFence);
		API_CHECK(vkResetFences);
		API_CHECK(vkGetFenceStatus);
		API_CHECK(vkWaitForFences);
		API_CHECK(vkCreateSemaphore);
		API_CHECK(vkDestroySemaphore);
		API_CHECK(vkCreateEvent);
		API_CHECK(vkDestroyEvent);
		API_CHECK(vkGetEventStatus);
		API_CHECK(vkSetEvent);
		API_CHECK(vkResetEvent);
		API_CHECK(vkCreateQueryPool);
		API_CHECK(vkDestroyQueryPool);
		API_CHECK(vkGetQueryPoolResults);
		API_CHECK(vkCreateBuffer);
		API_CHECK(vkDestroyBuffer);
		API_CHECK(vkCreateBufferView);
		API_CHECK(vkDestroyBufferView);
		API_CHECK(vkCreateImage);
		API_CHECK(vkDestroyImage);
		API_CHECK(vkGetImageSubresourceLayout);
		API_CHECK(vkCreateImageView);
		API_CHECK(vkDestroyImageView);
		API_CHECK(vkCreateShaderModule);
		API_CHECK(vkDestroyShaderModule);
		API_CHECK(vkCreatePipelineCache);
		API_CHECK(vkDestroyPipelineCache);
		API_CHECK(vkGetPipelineCacheData);
		API_CHECK(vkMergePipelineCaches);
		API_CHECK(vkCreateGraphicsPipelines);
		API_CHECK(vkCreateComputePipelines);
		API_CHECK(vkDestroyPipeline);
		API_CHECK(vkCreatePipelineLayout);
		API_CHECK(vkDestroyPipelineLayout);
		API_CHECK(vkCreateSampler);
		API_CHECK(vkDestroySampler);
		API_CHECK(vkCreateDescriptorSetLayout);
		API_CHECK(vkDestroyDescriptorSetLayout);
		API_CHECK(vkCreateDescriptorPool);
		API_CHECK(vkDestroyDescriptorPool);
		API_CHECK(vkResetDescriptorPool);
		API_CHECK(vkAllocateDescriptorSets);
		API_CHECK(vkFreeDescriptorSets);
		API_CHECK(vkUpdateDescriptorSets);
		API_CHECK(vkCreateFramebuffer);
		API_CHECK(vkDestroyFramebuffer);
		API_CHECK(vkCreateRenderPass);
		API_CHECK(vkDestroyRenderPass);
		API_CHECK(vkGetRenderAreaGranularity);
		API_CHECK(vkCreateCommandPool);
		API_CHECK(vkDestroyCommandPool);
		API_CHECK(vkResetCommandPool);
		API_CHECK(vkAllocateCommandBuffers);
		API_CHECK(vkFreeCommandBuffers);
		API_CHECK(vkBeginCommandBuffer);
		API_CHECK(vkEndCommandBuffer);
		API_CHECK(vkResetCommandBuffer);
		API_CHECK(vkCmdBindPipeline);
		API_CHECK(vkCmdSetViewport);
		API_CHECK(vkCmdSetScissor);
		API_CHECK(vkCmdSetLineWidth);
		API_CHECK(vkCmdSetDepthBias);
		API_CHECK(vkCmdSetBlendConstants);
		API_CHECK(vkCmdSetDepthBounds);
		API_CHECK(vkCmdSetStencilCompareMask);
		API_CHECK(vkCmdSetStencilWriteMask);
		API_CHECK(vkCmdSetStencilReference);
		API_CHECK(vkCmdBindDescriptorSets);
		API_CHECK(vkCmdBindIndexBuffer);
		API_CHECK(vkCmdBindVertexBuffers);
		API_CHECK(vkCmdDraw);
		API_CHECK(vkCmdDrawIndexed);
		API_CHECK(vkCmdDrawIndirect);
		API_CHECK(vkCmdDrawIndexedIndirect);
		API_CHECK(vkCmdDispatch);
		API_CHECK(vkCmdDispatchIndirect);
		API_CHECK(vkCmdCopyBuffer);
		API_CHECK(vkCmdCopyImage);
		API_CHECK(vkCmdBlitImage);
		API_CHECK(vkCmdCopyBufferToImage);
		API_CHECK(vkCmdCopyImageToBuffer);
		API_CHECK(vkCmdUpdateBuffer);
		API_CHECK(vkCmdFillBuffer);
		API_CHECK(vkCmdClearColorImage);
		API_CHECK(vkCmdClearDepthStencilImage);
		API_CHECK(vkCmdClearAttachments);
		API_CHECK(vkCmdResolveImage);
		API_CHECK(vkCmdSetEvent);
		API_CHECK(vkCmdResetEvent);
		API_CHECK(vkCmdWaitEvents);
		API_CHECK(vkCmdPipelineBarrier);
		API_CHECK(vkCmdBeginQuery);
		API_CHECK(vkCmdEndQuery);
		API_CHECK(vkCmdResetQueryPool);
		API_CHECK(vkCmdWriteTimestamp);
		API_CHECK(vkCmdCopyQueryPoolResults);
		API_CHECK(vkCmdPushConstants);
		API_CHECK(vkCmdBeginRenderPass);
		API_CHECK(vkCmdNextSubpass);
		API_CHECK(vkCmdEndRenderPass);
		API_CHECK(vkCmdExecuteCommands);
		API_CHECK(vkEnumerateInstanceVersion);
		API_CHECK(vkBindBufferMemory2);
		API_CHECK(vkBindImageMemory2);
		API_CHECK(vkGetDeviceGroupPeerMemoryFeatures);
		API_CHECK(vkCmdSetDeviceMask);
		API_CHECK(vkCmdDispatchBase);
		API_CHECK(vkEnumeratePhysicalDeviceGroups);
		API_CHECK(vkGetImageMemoryRequirements2);
		API_CHECK(vkGetBufferMemoryRequirements2);
		API_CHECK(vkGetImageSparseMemoryRequirements2);
		API_CHECK(vkGetPhysicalDeviceFeatures2);
		API_CHECK(vkGetPhysicalDeviceProperties2);
		API_CHECK(vkGetPhysicalDeviceFormatProperties2);
		API_CHECK(vkGetPhysicalDeviceImageFormatProperties2);
		API_CHECK(vkGetPhysicalDeviceQueueFamilyProperties2);
		API_CHECK(vkGetPhysicalDeviceMemoryProperties2);
		API_CHECK(vkGetPhysicalDeviceSparseImageFormatProperties2);
		API_CHECK(vkTrimCommandPool);
		API_CHECK(vkGetDeviceQueue2);
		API_CHECK(vkCreateSamplerYcbcrConversion);
		API_CHECK(vkDestroySamplerYcbcrConversion);
		API_CHECK(vkCreateDescriptorUpdateTemplate);
		API_CHECK(vkDestroyDescriptorUpdateTemplate);
		API_CHECK(vkUpdateDescriptorSetWithTemplate);
		API_CHECK(vkGetPhysicalDeviceExternalBufferProperties);
		API_CHECK(vkGetPhysicalDeviceExternalFenceProperties);
		API_CHECK(vkGetPhysicalDeviceExternalSemaphoreProperties);
		API_CHECK(vkGetDescriptorSetLayoutSupport);
		// VK_KHR_bind_memory2
		API_CHECK(vkBindBufferMemory2KHR);
		API_CHECK(vkBindImageMemory2KHR);
		// VK_KHR_descriptor_update_template
		API_CHECK(vkCreateDescriptorUpdateTemplateKHR);
		API_CHECK(vkDestroyDescriptorUpdateTemplateKHR);
		API_CHECK(vkUpdateDescriptorSetWithTemplateKHR);
		// VK_KHR_device_group
		API_CHECK(vkGetDeviceGroupPeerMemoryFeaturesKHR);
		API_CHECK(vkCmdSetDeviceMaskKHR);
		API_CHECK(vkCmdDispatchBaseKHR);
		// VK_KHR_device_group_creation
		API_CHECK(vkEnumeratePhysicalDeviceGroupsKHR);
		// VK_KHR_external_fence_capabilities
		API_CHECK(vkGetPhysicalDeviceExternalFencePropertiesKHR);
		// VK_KHR_external_memory_capabilities
		API_CHECK(vkGetPhysicalDeviceExternalBufferPropertiesKHR);
		// VK_KHR_external_semaphore_capabilities
		API_CHECK(vkGetPhysicalDeviceExternalSemaphorePropertiesKHR);
		// VK_KHR_get_memory_requirements2
		API_CHECK(vkGetImageMemoryRequirements2KHR);
		API_CHECK(vkGetBufferMemoryRequirements2KHR);
		API_CHECK(vkGetImageSparseMemoryRequirements2KHR);
		// VK_KHR_get_physical_device_properties2
		API_CHECK(vkGetPhysicalDeviceFeatures2KHR);
		API_CHECK(vkGetPhysicalDeviceProperties2KHR);
		API_CHECK(vkGetPhysicalDeviceFormatProperties2KHR);
		API_CHECK(vkGetPhysicalDeviceImageFormatProperties2KHR);
		API_CHECK(vkGetPhysicalDeviceQueueFamilyProperties2KHR);
		API_CHECK(vkGetPhysicalDeviceMemoryProperties2KHR);
		API_CHECK(vkGetPhysicalDeviceSparseImageFormatProperties2KHR);
		// VK_KHR_maintenance1
		API_CHECK(vkTrimCommandPoolKHR);
		// VK_KHR_maintenance3
		API_CHECK(vkGetDescriptorSetLayoutSupportKHR);
		// VK_KHR_sampler_ycbcr_conversion
		API_CHECK(vkCreateSamplerYcbcrConversionKHR);
		API_CHECK(vkDestroySamplerYcbcrConversionKHR);

		#undef API_CHECK

		auto bad_function = vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, "bad_function");
		EXPECT_EQ(bad_function, nullptr);
	}
}

TEST_F(SwiftShaderVulkanTest, Version)
{
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
	VkInstance instance;
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
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
