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

#include "VkBuffer.hpp"
#include "VkBufferView.hpp"
#include "VkCommandBuffer.hpp"
#include "VkCommandPool.hpp"
#include "VkConfig.h"
#include "VkDebug.hpp"
#include "VkDescriptorPool.hpp"
#include "VkDescriptorSetLayout.hpp"
#include "VkDescriptorUpdateTemplate.hpp"
#include "VkDestroy.h"
#include "VkDevice.hpp"
#include "VkDeviceMemory.hpp"
#include "VkEvent.hpp"
#include "VkFence.hpp"
#include "VkFramebuffer.hpp"
#include "VkGetProcAddress.h"
#include "VkImage.hpp"
#include "VkImageView.hpp"
#include "VkInstance.hpp"
#include "VkPhysicalDevice.hpp"
#include "VkPipeline.hpp"
#include "VkPipelineCache.hpp"
#include "VkPipelineLayout.hpp"
#include "VkQueryPool.hpp"
#include "VkQueue.hpp"
#include "VkSampler.hpp"
#include "VkSemaphore.hpp"
#include "VkShaderModule.hpp"
#include "VkRenderPass.hpp"

#ifdef VK_USE_PLATFORM_XLIB_KHR
#include "WSI/XlibSurfaceKHR.hpp"
#endif

#include "WSI/VkSwapchainKHR.hpp"

#include <algorithm>
#include <cstring>
#include <string>

namespace
{

bool HasExtensionProperty(const char* extensionName, const VkExtensionProperties* extensionProperties, uint32_t extensionPropertiesCount)
{
	for(uint32_t j = 0; j < extensionPropertiesCount; ++j)
	{
		if(strcmp(extensionName, extensionProperties[j].extensionName) == 0)
		{
			return true;
		}
	}

	return false;
}

}

extern "C"
{
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vk_icdGetInstanceProcAddr(VkInstance instance, const char* pName)
{
	TRACE("(VkInstance instance = 0x%X, const char* pName = 0x%X)", instance, pName);

	return vk::GetInstanceProcAddr(instance, pName);
}

VKAPI_ATTR VkResult VKAPI_CALL vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t* pSupportedVersion)
{
	*pSupportedVersion = 3;
	return VK_SUCCESS;
}

static const VkExtensionProperties instanceExtensionProperties[] =
{
	{ VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME, VK_KHR_DEVICE_GROUP_CREATION_SPEC_VERSION },
	{ VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME, VK_KHR_EXTERNAL_FENCE_CAPABILITIES_SPEC_VERSION },
	{ VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME, VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_SPEC_VERSION },
	{ VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME, VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_SPEC_VERSION },
	{ VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_SPEC_VERSION },
#ifndef __ANDROID__
	{ VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_SURFACE_SPEC_VERSION },
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
	{ VK_KHR_XLIB_SURFACE_EXTENSION_NAME, VK_KHR_XLIB_SURFACE_SPEC_VERSION },
#endif
};

static const VkExtensionProperties deviceExtensionProperties[] =
{
	{ VK_KHR_16BIT_STORAGE_EXTENSION_NAME, VK_KHR_16BIT_STORAGE_SPEC_VERSION },
	{ VK_KHR_BIND_MEMORY_2_EXTENSION_NAME, VK_KHR_BIND_MEMORY_2_SPEC_VERSION },
	{ VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME, VK_KHR_DEDICATED_ALLOCATION_SPEC_VERSION },
	{ VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME, VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_SPEC_VERSION },
	{ VK_KHR_DEVICE_GROUP_EXTENSION_NAME,  VK_KHR_DEVICE_GROUP_SPEC_VERSION },
	{ VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME, VK_KHR_EXTERNAL_FENCE_SPEC_VERSION },
	{ VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME, VK_KHR_EXTERNAL_MEMORY_SPEC_VERSION },
	{ VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME, VK_KHR_EXTERNAL_SEMAPHORE_SPEC_VERSION },
	{ VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME, VK_KHR_GET_MEMORY_REQUIREMENTS_2_SPEC_VERSION },
	{ VK_KHR_MAINTENANCE1_EXTENSION_NAME, VK_KHR_MAINTENANCE1_SPEC_VERSION },
	{ VK_KHR_MAINTENANCE2_EXTENSION_NAME, VK_KHR_MAINTENANCE2_SPEC_VERSION },
	{ VK_KHR_MAINTENANCE3_EXTENSION_NAME, VK_KHR_MAINTENANCE3_SPEC_VERSION },
	{ VK_KHR_MULTIVIEW_EXTENSION_NAME, VK_KHR_MULTIVIEW_SPEC_VERSION },
	{ VK_KHR_RELAXED_BLOCK_LAYOUT_EXTENSION_NAME, VK_KHR_RELAXED_BLOCK_LAYOUT_SPEC_VERSION },
	{ VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME, VK_KHR_SAMPLER_YCBCR_CONVERSION_SPEC_VERSION },
	{ VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME, VK_KHR_SHADER_DRAW_PARAMETERS_SPEC_VERSION },
	{ VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME, VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_SPEC_VERSION },
	{ VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME, VK_KHR_VARIABLE_POINTERS_SPEC_VERSION },
#ifndef __ANDROID__
	{ VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SWAPCHAIN_SPEC_VERSION },
#endif
};

VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance)
{
	TRACE("(const VkInstanceCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkInstance* pInstance = 0x%X)",
			pCreateInfo, pAllocator, pInstance);

	if(pCreateInfo->enabledLayerCount)
	{
		UNIMPLEMENTED("pCreateInfo->enabledLayerCount");
	}

	uint32_t extensionPropertiesCount = sizeof(instanceExtensionProperties) / sizeof(instanceExtensionProperties[0]);
	for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; ++i)
	{
		if (!HasExtensionProperty(pCreateInfo->ppEnabledExtensionNames[i], instanceExtensionProperties, extensionPropertiesCount))
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	if(pCreateInfo->pNext)
	{
		switch(*reinterpret_cast<const VkStructureType*>(pCreateInfo->pNext))
		{
		case VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO:
			// According to the Vulkan spec, section 2.7.2. Implicit Valid Usage:
			// "The values VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO and
			//  VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO are reserved for
			//  internal use by the loader, and do not have corresponding
			//  Vulkan structures in this Specification."
			break;
		default:
			UNIMPLEMENTED("pCreateInfo->pNext");
		}
	}

	*pInstance = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	VkResult result = vk::DispatchablePhysicalDevice::Create(pAllocator, pCreateInfo, &physicalDevice);
	if(result != VK_SUCCESS)
	{
		return result;
	}

	vk::Instance::CreateInfo info =
	{
		pCreateInfo,
		physicalDevice
	};

	result = vk::DispatchableInstance::Create(pAllocator, &info, pInstance);
	if(result != VK_SUCCESS)
	{
		vk::destroy(physicalDevice, pAllocator);
		return result;
	}

	return result;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkInstance instance = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)", instance, pAllocator);

	vk::destroy(instance, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices)
{
	TRACE("(VkInstance instance = 0x%X, uint32_t* pPhysicalDeviceCount = 0x%X, VkPhysicalDevice* pPhysicalDevices = 0x%X)",
		    instance, pPhysicalDeviceCount, pPhysicalDevices);

	if(!pPhysicalDevices)
	{
		*pPhysicalDeviceCount = vk::Cast(instance)->getPhysicalDeviceCount();
	}
	else
	{
		vk::Cast(instance)->getPhysicalDevices(*pPhysicalDeviceCount, pPhysicalDevices);
	}

	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, VkPhysicalDeviceFeatures* pFeatures = 0x%X)",
			physicalDevice, pFeatures);

	*pFeatures = vk::Cast(physicalDevice)->getFeatures();
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties* pFormatProperties)
{
	TRACE("GetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice = 0x%X, VkFormat format = %d, VkFormatProperties* pFormatProperties = 0x%X)",
			physicalDevice, (int)format, pFormatProperties);

	vk::Cast(physicalDevice)->getFormatProperties(format, pFormatProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, VkFormat format = %d, VkImageType type = %d, VkImageTiling tiling = %d, VkImageUsageFlags usage = %d, VkImageCreateFlags flags = %d, VkImageFormatProperties* pImageFormatProperties = 0x%X)",
			physicalDevice, (int)format, (int)type, (int)tiling, usage, flags, pImageFormatProperties);

	VkFormatProperties properties;
	vk::Cast(physicalDevice)->getFormatProperties(format, &properties);

	switch (tiling)
	{
	case VK_IMAGE_TILING_LINEAR:
		if (properties.linearTilingFeatures == 0) return VK_ERROR_FORMAT_NOT_SUPPORTED;
		break;

	case VK_IMAGE_TILING_OPTIMAL:
		if (properties.optimalTilingFeatures == 0) return VK_ERROR_FORMAT_NOT_SUPPORTED;
		break;

	default:
		UNIMPLEMENTED("tiling");
	}

	vk::Cast(physicalDevice)->getImageFormatProperties(format, type, tiling, usage, flags, pImageFormatProperties);

	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, VkPhysicalDeviceProperties* pProperties = 0x%X)",
		    physicalDevice, pProperties);

	*pProperties = vk::Cast(physicalDevice)->getProperties();
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, uint32_t* pQueueFamilyPropertyCount = 0x%X, VkQueueFamilyProperties* pQueueFamilyProperties = 0x%X))", physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);

	if(!pQueueFamilyProperties)
	{
		*pQueueFamilyPropertyCount = vk::Cast(physicalDevice)->getQueueFamilyPropertyCount();
	}
	else
	{
		vk::Cast(physicalDevice)->getQueueFamilyProperties(*pQueueFamilyPropertyCount, pQueueFamilyProperties);
	}
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, VkPhysicalDeviceMemoryProperties* pMemoryProperties = 0x%X)", physicalDevice, pMemoryProperties);

	*pMemoryProperties = vk::Cast(physicalDevice)->getMemoryProperties();
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char* pName)
{
	TRACE("(VkInstance instance = 0x%X, const char* pName = 0x%X)", instance, pName);

	return vk::GetInstanceProcAddr(instance, pName);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice device, const char* pName)
{
	TRACE("(VkDevice device = 0x%X, const char* pName = 0x%X)", device, pName);

	return vk::GetDeviceProcAddr(device, pName);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, const VkDeviceCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkDevice* pDevice = 0x%X)",
		physicalDevice, pCreateInfo, pAllocator, pDevice);

	if(pCreateInfo->enabledLayerCount)
	{
		// "The ppEnabledLayerNames and enabledLayerCount members of VkDeviceCreateInfo are deprecated and their values must be ignored by implementations."
		UNIMPLEMENTED("pCreateInfo->enabledLayerCount");   // TODO(b/119321052): UNIMPLEMENTED() should be used only for features that must still be implemented. Use a more informational macro here.
	}

	uint32_t extensionPropertiesCount = sizeof(deviceExtensionProperties) / sizeof(deviceExtensionProperties[0]);
	for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; ++i)
	{
		if (!HasExtensionProperty(pCreateInfo->ppEnabledExtensionNames[i], deviceExtensionProperties, extensionPropertiesCount))
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	const VkBaseInStructure* extensionCreateInfo = reinterpret_cast<const VkBaseInStructure*>(pCreateInfo->pNext);

	while(extensionCreateInfo)
	{
		switch(extensionCreateInfo->sType)
		{
		case VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO:
			// According to the Vulkan spec, section 2.7.2. Implicit Valid Usage:
			// "The values VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO and
			//  VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO are reserved for
			//  internal use by the loader, and do not have corresponding
			//  Vulkan structures in this Specification."
			break;
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2:
			{
				ASSERT(!pCreateInfo->pEnabledFeatures);   // "If the pNext chain includes a VkPhysicalDeviceFeatures2 structure, then pEnabledFeatures must be NULL"

				const VkPhysicalDeviceFeatures2* physicalDeviceFeatures2 = reinterpret_cast<const VkPhysicalDeviceFeatures2*>(extensionCreateInfo);

				if(!vk::Cast(physicalDevice)->hasFeatures(physicalDeviceFeatures2->features))
				{
					return VK_ERROR_FEATURE_NOT_PRESENT;
				}
			}
			break;
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES:
			{
				const VkPhysicalDeviceSamplerYcbcrConversionFeatures* samplerYcbcrConversionFeatures = reinterpret_cast<const VkPhysicalDeviceSamplerYcbcrConversionFeatures*>(extensionCreateInfo);

				if(samplerYcbcrConversionFeatures->samplerYcbcrConversion == VK_TRUE)
				{
					return VK_ERROR_FEATURE_NOT_PRESENT;
				}
			}
			break;
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES:
			{
				const VkPhysicalDevice16BitStorageFeatures* storage16BitFeatures = reinterpret_cast<const VkPhysicalDevice16BitStorageFeatures*>(extensionCreateInfo);

				if(storage16BitFeatures->storageBuffer16BitAccess == VK_TRUE ||
				   storage16BitFeatures->uniformAndStorageBuffer16BitAccess == VK_TRUE ||
				   storage16BitFeatures->storagePushConstant16 == VK_TRUE ||
				   storage16BitFeatures->storageInputOutput16 == VK_TRUE)
				{
					return VK_ERROR_FEATURE_NOT_PRESENT;
				}
			}
			break;
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES:
			{
				const VkPhysicalDeviceVariablePointerFeatures* variablePointerFeatures = reinterpret_cast<const VkPhysicalDeviceVariablePointerFeatures*>(extensionCreateInfo);

				if(variablePointerFeatures->variablePointersStorageBuffer == VK_TRUE ||
				   variablePointerFeatures->variablePointers == VK_TRUE)
				{
					return VK_ERROR_FEATURE_NOT_PRESENT;
				}
			}
			break;
		case VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO:
			{
				const VkDeviceGroupDeviceCreateInfo* groupDeviceCreateInfo = reinterpret_cast<const VkDeviceGroupDeviceCreateInfo*>(extensionCreateInfo);

				if((groupDeviceCreateInfo->physicalDeviceCount != 1) ||
				   (groupDeviceCreateInfo->pPhysicalDevices[0] != physicalDevice))
				{
					return VK_ERROR_FEATURE_NOT_PRESENT;
				}
			}
			break;
		default:
			// "the [driver] must skip over, without processing (other than reading the sType and pNext members) any structures in the chain with sType values not defined by [supported extenions]"
			UNIMPLEMENTED("extensionCreateInfo->sType");   // TODO(b/119321052): UNIMPLEMENTED() should be used only for features that must still be implemented. Use a more informational macro here.
			break;
		}

		extensionCreateInfo = extensionCreateInfo->pNext;
	}

	ASSERT(pCreateInfo->queueCreateInfoCount > 0);

	if(pCreateInfo->pEnabledFeatures)
	{
		if(!vk::Cast(physicalDevice)->hasFeatures(*(pCreateInfo->pEnabledFeatures)))
		{
			UNIMPLEMENTED("pCreateInfo->pEnabledFeatures");
			return VK_ERROR_FEATURE_NOT_PRESENT;
		}
	}

	uint32_t queueFamilyPropertyCount = vk::Cast(physicalDevice)->getQueueFamilyPropertyCount();

	for(uint32_t i = 0; i < pCreateInfo->queueCreateInfoCount; i++)
	{
		const VkDeviceQueueCreateInfo& queueCreateInfo = pCreateInfo->pQueueCreateInfos[i];
		if(queueCreateInfo.pNext || queueCreateInfo.flags)
		{
			UNIMPLEMENTED("queueCreateInfo.pNext || queueCreateInfo.flags");
		}

		ASSERT(queueCreateInfo.queueFamilyIndex < queueFamilyPropertyCount);
		(void)queueFamilyPropertyCount; // Silence unused variable warning
	}

	vk::Device::CreateInfo deviceCreateInfo =
	{
		pCreateInfo,
		physicalDevice
	};

	return vk::DispatchableDevice::Create(pAllocator, &deviceCreateInfo, pDevice);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)", device, pAllocator);

	vk::destroy(device, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties)
{
	TRACE("(const char* pLayerName = 0x%X, uint32_t* pPropertyCount = 0x%X, VkExtensionProperties* pProperties = 0x%X)",
	      pLayerName, pPropertyCount, pProperties);

	uint32_t extensionPropertiesCount = sizeof(instanceExtensionProperties) / sizeof(instanceExtensionProperties[0]);

	if(!pProperties)
	{
		*pPropertyCount = extensionPropertiesCount;
		return VK_SUCCESS;
	}

	for(uint32_t i = 0; i < std::min(*pPropertyCount, extensionPropertiesCount); i++)
	{
		pProperties[i] = instanceExtensionProperties[i];
	}

	return (*pPropertyCount < extensionPropertiesCount) ? VK_INCOMPLETE : VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, const char* pLayerName, uint32_t* pPropertyCount = 0x%X, VkExtensionProperties* pProperties = 0x%X)", physicalDevice, pPropertyCount, pProperties);

	uint32_t extensionPropertiesCount = sizeof(deviceExtensionProperties) / sizeof(deviceExtensionProperties[0]);

	if(!pProperties)
	{
		*pPropertyCount = extensionPropertiesCount;
		return VK_SUCCESS;
	}

	for(uint32_t i = 0; i < std::min(*pPropertyCount, extensionPropertiesCount); i++)
	{
		pProperties[i] = deviceExtensionProperties[i];
	}

	return (*pPropertyCount < extensionPropertiesCount) ? VK_INCOMPLETE : VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties)
{
	TRACE("(uint32_t* pPropertyCount = 0x%X, VkLayerProperties* pProperties = 0x%X)", pPropertyCount, pProperties);

	if(!pProperties)
	{
		*pPropertyCount = 0;
		return VK_SUCCESS;
	}

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkLayerProperties* pProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, uint32_t* pPropertyCount = 0x%X, VkLayerProperties* pProperties = 0x%X)", physicalDevice, pPropertyCount, pProperties);

	if(!pProperties)
	{
		*pPropertyCount = 0;
		return VK_SUCCESS;
	}

	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue)
{
	TRACE("(VkDevice device = 0x%X, uint32_t queueFamilyIndex = %d, uint32_t queueIndex = %d, VkQueue* pQueue = 0x%X)",
		    device, queueFamilyIndex, queueIndex, pQueue);

	*pQueue = vk::Cast(device)->getQueue(queueFamilyIndex, queueIndex);
}

VKAPI_ATTR VkResult VKAPI_CALL vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence)
{
	TRACE("(VkQueue queue = 0x%X, uint32_t submitCount = %d, const VkSubmitInfo* pSubmits = 0x%X, VkFence fence = 0x%X)",
	      queue, submitCount, pSubmits, fence);

	vk::Cast(queue)->submit(submitCount, pSubmits, fence);

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkQueueWaitIdle(VkQueue queue)
{
	TRACE("(VkQueue queue = 0x%X)", queue);

	vk::Cast(queue)->waitIdle();

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkDeviceWaitIdle(VkDevice device)
{
	TRACE("(VkDevice device = 0x%X)", device);

	vk::Cast(device)->waitIdle();

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkAllocateMemory(VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory)
{
	TRACE("(VkDevice device = 0x%X, const VkMemoryAllocateInfo* pAllocateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkDeviceMemory* pMemory = 0x%X)",
		    device, pAllocateInfo, pAllocator, pMemory);

	const VkBaseOutStructure* allocationInfo = reinterpret_cast<const VkBaseOutStructure*>(pAllocateInfo->pNext);
	while(allocationInfo)
	{
		switch(allocationInfo->sType)
		{
		case VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO:
			// This can safely be ignored, as the Vulkan spec mentions:
			// "If the pNext chain includes a VkMemoryDedicatedAllocateInfo structure, then that structure
			//  includes a handle of the sole buffer or image resource that the memory *can* be bound to."
			break;
		default:
			UNIMPLEMENTED("allocationInfo->sType");
			break;
		}

		allocationInfo = allocationInfo->pNext;
	}

	VkResult result = vk::DeviceMemory::Create(pAllocator, pAllocateInfo, pMemory);
	if(result != VK_SUCCESS)
	{
		return result;
	}

	// Make sure the memory allocation is done now so that OOM errors can be checked now
	result = vk::Cast(*pMemory)->allocate();
	if(result != VK_SUCCESS)
	{
		vk::destroy(*pMemory, pAllocator);
		*pMemory = VK_NULL_HANDLE;
	}

	return result;
}

VKAPI_ATTR void VKAPI_CALL vkFreeMemory(VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkDeviceMemory memory = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
		    device, memory, pAllocator);

	vk::destroy(memory, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData)
{
	TRACE("(VkDevice device = 0x%X, VkDeviceMemory memory = 0x%X, VkDeviceSize offset = %d, VkDeviceSize size = %d, VkMemoryMapFlags flags = 0x%X, void** ppData = 0x%X)",
		    device, memory, offset, size, flags, ppData);

	return vk::Cast(memory)->map(offset, size, ppData);
}

VKAPI_ATTR void VKAPI_CALL vkUnmapMemory(VkDevice device, VkDeviceMemory memory)
{
	TRACE("(VkDevice device = 0x%X, VkDeviceMemory memory = 0x%X)", device, memory);

	// Noop, memory will be released when the DeviceMemory object is released
}

VKAPI_ATTR VkResult VKAPI_CALL vkFlushMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges)
{
	TRACE("(VkDevice device = 0x%X, uint32_t memoryRangeCount = %d, const VkMappedMemoryRange* pMemoryRanges = 0x%X)",
		    device, memoryRangeCount, pMemoryRanges);

	// Noop, host and device memory are the same to SwiftShader

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkInvalidateMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges)
{
	TRACE("(VkDevice device = 0x%X, uint32_t memoryRangeCount = %d, const VkMappedMemoryRange* pMemoryRanges = 0x%X)",
		    device, memoryRangeCount, pMemoryRanges);

	// Noop, host and device memory are the same to SwiftShader

	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetDeviceMemoryCommitment(VkDevice pDevice, VkDeviceMemory pMemory, VkDeviceSize* pCommittedMemoryInBytes)
{
	TRACE("(VkDevice device = 0x%X, VkDeviceMemory memory = 0x%X, VkDeviceSize* pCommittedMemoryInBytes = 0x%X)",
	      pDevice, pMemory, pCommittedMemoryInBytes);

	auto memory = vk::Cast(pMemory);

#if !defined(NDEBUG) || defined(DCHECK_ALWAYS_ON)
	const auto& memoryProperties = vk::Cast(vk::Cast(pDevice)->getPhysicalDevice())->getMemoryProperties();
	uint32_t typeIndex = memory->getMemoryTypeIndex();
	ASSERT(typeIndex < memoryProperties.memoryTypeCount);
	ASSERT(memoryProperties.memoryTypes[typeIndex].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT);
#endif

	*pCommittedMemoryInBytes = memory->getCommittedMemoryInBytes();
}

VKAPI_ATTR VkResult VKAPI_CALL vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset)
{
	TRACE("(VkDevice device = 0x%X, VkBuffer buffer = 0x%X, VkDeviceMemory memory = 0x%X, VkDeviceSize memoryOffset = %d)",
		    device, buffer, memory, memoryOffset);

	vk::Cast(buffer)->bind(memory, memoryOffset);

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkBindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset)
{
	TRACE("(VkDevice device = 0x%X, VkImage image = 0x%X, VkDeviceMemory memory = 0x%X, VkDeviceSize memoryOffset = %d)",
		    device, image, memory, memoryOffset);

	vk::Cast(image)->bind(memory, memoryOffset);

	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer, VkMemoryRequirements* pMemoryRequirements)
{
	TRACE("(VkDevice device = 0x%X, VkBuffer buffer = 0x%X, VkMemoryRequirements* pMemoryRequirements = 0x%X)",
		    device, buffer, pMemoryRequirements);

	*pMemoryRequirements = vk::Cast(buffer)->getMemoryRequirements();
}

VKAPI_ATTR void VKAPI_CALL vkGetImageMemoryRequirements(VkDevice device, VkImage image, VkMemoryRequirements* pMemoryRequirements)
{
	TRACE("(VkDevice device = 0x%X, VkImage image = 0x%X, VkMemoryRequirements* pMemoryRequirements = 0x%X)",
		    device, image, pMemoryRequirements);

	*pMemoryRequirements = vk::Cast(image)->getMemoryRequirements();
}

VKAPI_ATTR void VKAPI_CALL vkGetImageSparseMemoryRequirements(VkDevice device, VkImage image, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements* pSparseMemoryRequirements)
{
	TRACE("(VkDevice device, VkImage image, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements* pSparseMemoryRequirements)",
	      device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);

	// The 'sparseBinding' feature is not supported, so images can not be created with the VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT flag.
	// "If the image was not created with VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT then pSparseMemoryRequirementCount will be set to zero and pSparseMemoryRequirements will not be written to."
	*pSparseMemoryRequirementCount = 0;
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceSparseImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling, uint32_t* pPropertyCount, VkSparseImageFormatProperties* pProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling, uint32_t* pPropertyCount, VkSparseImageFormatProperties* pProperties)",
			physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);

	// We do not support sparse images.
	*pPropertyCount = 0;
}

VKAPI_ATTR VkResult VKAPI_CALL vkQueueBindSparse(VkQueue queue, uint32_t bindInfoCount, const VkBindSparseInfo* pBindInfo, VkFence fence)
{
	TRACE("()");
	UNIMPLEMENTED("vkQueueBindSparse");
	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateFence(VkDevice device, const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence)
{
	TRACE("(VkDevice device = 0x%X, const VkFenceCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkFence* pFence = 0x%X)",
		    device, pCreateInfo, pAllocator, pFence);

	if(pCreateInfo->pNext)
	{
		UNIMPLEMENTED("pCreateInfo->pNext");
	}

	return vk::Fence::Create(pAllocator, pCreateInfo, pFence);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyFence(VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkFence fence = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
		    device, fence, pAllocator);


	vk::destroy(fence, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences)
{
	TRACE("(VkDevice device = 0x%X, uint32_t fenceCount = %d, const VkFence* pFences = 0x%X)",
	      device, fenceCount, pFences);

	for(uint32_t i = 0; i < fenceCount; i++)
	{
		vk::Cast(pFences[i])->reset();
	}

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetFenceStatus(VkDevice device, VkFence fence)
{
	TRACE("(VkDevice device = 0x%X, VkFence fence = 0x%X)", device, fence);

	return vk::Cast(fence)->getStatus();
}

VKAPI_ATTR VkResult VKAPI_CALL vkWaitForFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout)
{
	TRACE("(VkDevice device = 0x%X, uint32_t fenceCount = %d, const VkFence* pFences = 0x%X, VkBool32 waitAll = %d, uint64_t timeout = %d)",
		device, fenceCount, pFences, waitAll, timeout);

	vk::Cast(device)->waitForFences(fenceCount, pFences, waitAll, timeout);

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore)
{
	TRACE("(VkDevice device = 0x%X, const VkSemaphoreCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkSemaphore* pSemaphore = 0x%X)",
	      device, pCreateInfo, pAllocator, pSemaphore);

	if(pCreateInfo->pNext || pCreateInfo->flags)
	{
		UNIMPLEMENTED("pCreateInfo->pNext || pCreateInfo->flags");
	}

	return vk::Semaphore::Create(pAllocator, pCreateInfo, pSemaphore);
}

VKAPI_ATTR void VKAPI_CALL vkDestroySemaphore(VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkSemaphore semaphore = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
	      device, semaphore, pAllocator);

	vk::destroy(semaphore, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateEvent(VkDevice device, const VkEventCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkEvent* pEvent)
{
	TRACE("(VkDevice device = 0x%X, const VkEventCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkEvent* pEvent = 0x%X)",
	      device, pCreateInfo, pAllocator, pEvent);

	if(pCreateInfo->pNext || pCreateInfo->flags)
	{
		UNIMPLEMENTED("pCreateInfo->pNext || pCreateInfo->flags");
	}

	return vk::Event::Create(pAllocator, pCreateInfo, pEvent);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyEvent(VkDevice device, VkEvent event, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkEvent event = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
	      device, event, pAllocator);

	vk::destroy(event, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetEventStatus(VkDevice device, VkEvent event)
{
	TRACE("(VkDevice device = 0x%X, VkEvent event = 0x%X)", device, event);

	return vk::Cast(event)->getStatus();
}

VKAPI_ATTR VkResult VKAPI_CALL vkSetEvent(VkDevice device, VkEvent event)
{
	TRACE("(VkDevice device = 0x%X, VkEvent event = 0x%X)", device, event);

	vk::Cast(event)->signal();

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetEvent(VkDevice device, VkEvent event)
{
	TRACE("(VkDevice device = 0x%X, VkEvent event = 0x%X)", device, event);

	vk::Cast(event)->reset();

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateQueryPool(VkDevice device, const VkQueryPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkQueryPool* pQueryPool)
{
	TRACE("(VkDevice device = 0x%X, const VkQueryPoolCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkQueryPool* pQueryPool = 0x%X)",
	      device, pCreateInfo, pAllocator, pQueryPool);

	if(pCreateInfo->pNext || pCreateInfo->flags)
	{
		UNIMPLEMENTED("pCreateInfo->pNext || pCreateInfo->flags");
	}

	return vk::QueryPool::Create(pAllocator, pCreateInfo, pQueryPool);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyQueryPool(VkDevice device, VkQueryPool queryPool, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkQueryPool queryPool = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
	      device, queryPool, pAllocator);

	vk::destroy(queryPool, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetQueryPoolResults(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, size_t dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags)
{
	TRACE("(VkDevice device = 0x%X, VkQueryPool queryPool = 0x%X, uint32_t firstQuery = %d, uint32_t queryCount = %d, size_t dataSize = %d, void* pData = 0x%X, VkDeviceSize stride = 0x%X, VkQueryResultFlags flags = %d)",
	      device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);

	vk::Cast(queryPool)->getResults(firstQuery, queryCount, dataSize, pData, stride, flags);

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateBuffer(VkDevice device, const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer)
{
	TRACE("(VkDevice device = 0x%X, const VkBufferCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkBuffer* pBuffer = 0x%X)",
		    device, pCreateInfo, pAllocator, pBuffer);

	if(pCreateInfo->pNext)
	{
		UNIMPLEMENTED("pCreateInfo->pNext");
	}

	return vk::Buffer::Create(pAllocator, pCreateInfo, pBuffer);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyBuffer(VkDevice device, VkBuffer buffer, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkBuffer buffer = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
		    device, buffer, pAllocator);

	vk::destroy(buffer, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateBufferView(VkDevice device, const VkBufferViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBufferView* pView)
{
	TRACE("(VkDevice device, const VkBufferViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBufferView* pView)",
		device, pCreateInfo, pAllocator, pView);

	if(pCreateInfo->pNext || pCreateInfo->flags)
	{
		UNIMPLEMENTED("pCreateInfo->pNext || pCreateInfo->flags");
	}

	return vk::BufferView::Create(pAllocator, pCreateInfo, pView);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyBufferView(VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks* pAllocator)",
	      device, bufferView, pAllocator);

	vk::destroy(bufferView, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateImage(VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage)
{
	TRACE("(VkDevice device = 0x%X, const VkImageCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkImage* pImage = 0x%X)",
		    device, pCreateInfo, pAllocator, pImage);

	if(pCreateInfo->pNext)
	{
		UNIMPLEMENTED("pCreateInfo->pNext");
	}

	vk::Image::CreateInfo imageCreateInfo =
	{
		pCreateInfo,
		device
	};

	return vk::Image::Create(pAllocator, &imageCreateInfo, pImage);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkImage image = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
		    device, image, pAllocator);

	vk::destroy(image, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL vkGetImageSubresourceLayout(VkDevice device, VkImage image, const VkImageSubresource* pSubresource, VkSubresourceLayout* pLayout)
{
	TRACE("(VkDevice device, VkImage image, const VkImageSubresource* pSubresource, VkSubresourceLayout* pLayout)",
		device, image, pSubresource, pLayout);

	vk::Cast(image)->getSubresourceLayout(pSubresource, pLayout);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateImageView(VkDevice device, const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pView)
{
	TRACE("(VkDevice device = 0x%X, const VkImageViewCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkImageView* pView = 0x%X)",
		    device, pCreateInfo, pAllocator, pView);

	if(pCreateInfo->flags)
	{
		UNIMPLEMENTED("pCreateInfo->flags");
	}

	const VkBaseInStructure* extensionCreateInfo = reinterpret_cast<const VkBaseInStructure*>(pCreateInfo->pNext);

	while(extensionCreateInfo)
	{
		switch(extensionCreateInfo->sType)
		{
		case VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO_KHR:
		{
			const VkImageViewUsageCreateInfo* multiviewCreateInfo = reinterpret_cast<const VkImageViewUsageCreateInfo*>(extensionCreateInfo);
			ASSERT(!(~vk::Cast(pCreateInfo->image)->getUsage() & multiviewCreateInfo->usage));
		}
		break;
		case VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO:
		{
			const VkSamplerYcbcrConversionInfo* ycbcrConversionInfo = reinterpret_cast<const VkSamplerYcbcrConversionInfo*>(extensionCreateInfo);
			if(ycbcrConversionInfo->conversion != VK_NULL_HANDLE)
			{
				ASSERT((pCreateInfo->components.r == VK_COMPONENT_SWIZZLE_IDENTITY) &&
				       (pCreateInfo->components.g == VK_COMPONENT_SWIZZLE_IDENTITY) &&
				       (pCreateInfo->components.b == VK_COMPONENT_SWIZZLE_IDENTITY) &&
				       (pCreateInfo->components.a == VK_COMPONENT_SWIZZLE_IDENTITY));
			}
		}
		break;
		default:
			UNIMPLEMENTED("extensionCreateInfo->sType");
			break;
		}

		extensionCreateInfo = extensionCreateInfo->pNext;
	}

	return vk::ImageView::Create(pAllocator, pCreateInfo, pView);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyImageView(VkDevice device, VkImageView imageView, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkImageView imageView = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
	      device, imageView, pAllocator);

	vk::destroy(imageView, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule)
{
	TRACE("(VkDevice device = 0x%X, const VkShaderModuleCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkShaderModule* pShaderModule = 0x%X)",
		    device, pCreateInfo, pAllocator, pShaderModule);

	if(pCreateInfo->pNext || pCreateInfo->flags)
	{
		UNIMPLEMENTED("pCreateInfo->pNext || pCreateInfo->flags");
	}

	return vk::ShaderModule::Create(pAllocator, pCreateInfo, pShaderModule);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkShaderModule shaderModule = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
		    device, shaderModule, pAllocator);

	vk::destroy(shaderModule, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineCache* pPipelineCache)
{
	TRACE("(VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineCache* pPipelineCache)",
	      device, pCreateInfo, pAllocator, pPipelineCache);

	if(pCreateInfo->pNext || pCreateInfo->flags)
	{
		UNIMPLEMENTED("pCreateInfo->pNext || pCreateInfo->flags");
	}

	return vk::PipelineCache::Create(pAllocator, pCreateInfo, pPipelineCache);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device, VkPipelineCache pipelineCache, const VkAllocationCallbacks* pAllocator)",
	      device, pipelineCache, pAllocator);

	vk::destroy(pipelineCache, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineCacheData(VkDevice device, VkPipelineCache pipelineCache, size_t* pDataSize, void* pData)
{
	TRACE("()");
	UNIMPLEMENTED("vkGetPipelineCacheData");
	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkMergePipelineCaches(VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount, const VkPipelineCache* pSrcCaches)
{
	TRACE("()");
	UNIMPLEMENTED("vkMergePipelineCaches");
	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines)
{
	TRACE("(VkDevice device = 0x%X, VkPipelineCache pipelineCache = 0x%X, uint32_t createInfoCount = %d, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator = 0x%X, VkPipeline* pPipelines = 0x%X)",
		    device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);

	// TODO (b/123588002): Optimize based on pipelineCache.

	VkResult errorResult = VK_SUCCESS;
	for(uint32_t i = 0; i < createInfoCount; i++)
	{
		VkResult result = vk::GraphicsPipeline::Create(pAllocator, &pCreateInfos[i], &pPipelines[i]);
		if(result == VK_SUCCESS)
		{
			static_cast<vk::GraphicsPipeline*>(vk::Cast(pPipelines[i]))->compileShaders(pAllocator, &pCreateInfos[i]);
		}
		else
		{
			// According to the Vulkan spec, section 9.4. Multiple Pipeline Creation
			// "When an application attempts to create many pipelines in a single command,
			//  it is possible that some subset may fail creation. In that case, the
			//  corresponding entries in the pPipelines output array will be filled with
			//  VK_NULL_HANDLE values. If any pipeline fails creation (for example, due to
			//  out of memory errors), the vkCreate*Pipelines commands will return an
			//  error code. The implementation will attempt to create all pipelines, and
			//  only return VK_NULL_HANDLE values for those that actually failed."
			pPipelines[i] = VK_NULL_HANDLE;
			errorResult = result;
		}
	}

	return errorResult;
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines)
{
	TRACE("(VkDevice device = 0x%X, VkPipelineCache pipelineCache = 0x%X, uint32_t createInfoCount = %d, const VkComputePipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator = 0x%X, VkPipeline* pPipelines = 0x%X)",
		device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);

	// TODO (b/123588002): Optimize based on pipelineCache.

	VkResult errorResult = VK_SUCCESS;
	for(uint32_t i = 0; i < createInfoCount; i++)
	{
		VkResult result = vk::ComputePipeline::Create(pAllocator, &pCreateInfos[i], &pPipelines[i]);
		if(result == VK_SUCCESS)
		{
			static_cast<vk::ComputePipeline*>(vk::Cast(pPipelines[i]))->compileShaders(pAllocator, &pCreateInfos[i]);
		}
		else
		{
			// According to the Vulkan spec, section 9.4. Multiple Pipeline Creation
			// "When an application attempts to create many pipelines in a single command,
			//  it is possible that some subset may fail creation. In that case, the
			//  corresponding entries in the pPipelines output array will be filled with
			//  VK_NULL_HANDLE values. If any pipeline fails creation (for example, due to
			//  out of memory errors), the vkCreate*Pipelines commands will return an
			//  error code. The implementation will attempt to create all pipelines, and
			//  only return VK_NULL_HANDLE values for those that actually failed."
			pPipelines[i] = VK_NULL_HANDLE;
			errorResult = result;
		}
	}

	return errorResult;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyPipeline(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkPipeline pipeline = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
		    device, pipeline, pAllocator);

	vk::destroy(pipeline, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineLayout* pPipelineLayout)
{
	TRACE("(VkDevice device = 0x%X, const VkPipelineLayoutCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkPipelineLayout* pPipelineLayout = 0x%X)",
		    device, pCreateInfo, pAllocator, pPipelineLayout);

	if(pCreateInfo->pNext || pCreateInfo->flags)
	{
		UNIMPLEMENTED("pCreateInfo->pNext || pCreateInfo->flags");
	}

	return vk::PipelineLayout::Create(pAllocator, pCreateInfo, pPipelineLayout);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkPipelineLayout pipelineLayout = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
		    device, pipelineLayout, pAllocator);

	vk::destroy(pipelineLayout, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateSampler(VkDevice device, const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSampler* pSampler)
{
	TRACE("(VkDevice device = 0x%X, const VkSamplerCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkSampler* pSampler = 0x%X)",
		    device, pCreateInfo, pAllocator, pSampler);

	if(pCreateInfo->pNext || pCreateInfo->flags)
	{
		UNIMPLEMENTED("pCreateInfo->pNext || pCreateInfo->flags");
	}

	return vk::Sampler::Create(pAllocator, pCreateInfo, pSampler);
}

VKAPI_ATTR void VKAPI_CALL vkDestroySampler(VkDevice device, VkSampler sampler, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkSampler sampler = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
		    device, sampler, pAllocator);

	vk::destroy(sampler, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout)
{
	TRACE("(VkDevice device = 0x%X, const VkDescriptorSetLayoutCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkDescriptorSetLayout* pSetLayout = 0x%X)",
	      device, pCreateInfo, pAllocator, pSetLayout);

	if(pCreateInfo->pNext)
	{
		UNIMPLEMENTED("pCreateInfo->pNext");
	}

	return vk::DescriptorSetLayout::Create(pAllocator, pCreateInfo, pSetLayout);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkDescriptorSetLayout descriptorSetLayout = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
	      device, descriptorSetLayout, pAllocator);

	vk::destroy(descriptorSetLayout, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorPool* pDescriptorPool)
{
	TRACE("(VkDevice device = 0x%X, const VkDescriptorPoolCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkDescriptorPool* pDescriptorPool = 0x%X)",
	      device, pCreateInfo, pAllocator, pDescriptorPool);

	if(pCreateInfo->pNext)
	{
		UNIMPLEMENTED("pCreateInfo->pNext");
	}

	return vk::DescriptorPool::Create(pAllocator, pCreateInfo, pDescriptorPool);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkDescriptorPool descriptorPool = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
	      device, descriptorPool, pAllocator);

	vk::destroy(descriptorPool, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags)
{
	TRACE("(VkDevice device = 0x%X, VkDescriptorPool descriptorPool = 0x%X, VkDescriptorPoolResetFlags flags = 0x%X)",
		device, descriptorPool, flags);

	if(flags)
	{
		UNIMPLEMENTED("flags");
	}

	return vk::Cast(descriptorPool)->reset();
}

VKAPI_ATTR VkResult VKAPI_CALL vkAllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets)
{
	TRACE("(VkDevice device = 0x%X, const VkDescriptorSetAllocateInfo* pAllocateInfo = 0x%X, VkDescriptorSet* pDescriptorSets = 0x%X)",
		device, pAllocateInfo, pDescriptorSets);

	if(pAllocateInfo->pNext)
	{
		UNIMPLEMENTED("pAllocateInfo->pNext");
	}

	return vk::Cast(pAllocateInfo->descriptorPool)->allocateSets(
		pAllocateInfo->descriptorSetCount, pAllocateInfo->pSetLayouts, pDescriptorSets);
}

VKAPI_ATTR VkResult VKAPI_CALL vkFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets)
{
	TRACE("(VkDevice device = 0x%X, VkDescriptorPool descriptorPool = 0x%X, uint32_t descriptorSetCount = %d, const VkDescriptorSet* pDescriptorSets = 0x%X)",
		device, descriptorPool, descriptorSetCount, pDescriptorSets);

	vk::Cast(descriptorPool)->freeSets(descriptorSetCount, pDescriptorSets);

	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkUpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies)
{
	TRACE("(VkDevice device = 0x%X, uint32_t descriptorWriteCount = %d, const VkWriteDescriptorSet* pDescriptorWrites = 0x%X, uint32_t descriptorCopyCount = %d, const VkCopyDescriptorSet* pDescriptorCopies = 0x%X)",
	      device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);

	vk::Cast(device)->updateDescriptorSets(descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateFramebuffer(VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer)
{
	TRACE("(VkDevice device = 0x%X, const VkFramebufferCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkFramebuffer* pFramebuffer = 0x%X)",
		    device, pCreateInfo, pAllocator, pFramebuffer);

	if(pCreateInfo->pNext || pCreateInfo->flags)
	{
		UNIMPLEMENTED("pCreateInfo->pNext || pCreateInfo->flags");
	}

	return vk::Framebuffer::Create(pAllocator, pCreateInfo, pFramebuffer);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkFramebuffer framebuffer = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)");

	vk::destroy(framebuffer, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateRenderPass(VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass)
{
	TRACE("(VkDevice device = 0x%X, const VkRenderPassCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkRenderPass* pRenderPass = 0x%X)",
		    device, pCreateInfo, pAllocator, pRenderPass);

	if(pCreateInfo->flags)
	{
		UNIMPLEMENTED("pCreateInfo->flags");
	}

	const VkBaseInStructure* extensionCreateInfo = reinterpret_cast<const VkBaseInStructure*>(pCreateInfo->pNext);

	while(extensionCreateInfo)
	{
		switch(extensionCreateInfo->sType)
		{
		case VK_STRUCTURE_TYPE_RENDER_PASS_INPUT_ATTACHMENT_ASPECT_CREATE_INFO:
		{
			const VkRenderPassInputAttachmentAspectCreateInfo* inputAttachmentAspectCreateInfo = reinterpret_cast<const VkRenderPassInputAttachmentAspectCreateInfo*>(extensionCreateInfo);

			for(uint32_t i = 0; i < inputAttachmentAspectCreateInfo->aspectReferenceCount; i++)
			{
				const VkInputAttachmentAspectReference& aspectReference = inputAttachmentAspectCreateInfo->pAspectReferences[i];
				ASSERT(aspectReference.subpass < pCreateInfo->subpassCount);
				const VkSubpassDescription& subpassDescription = pCreateInfo->pSubpasses[aspectReference.subpass];
				ASSERT(aspectReference.inputAttachmentIndex < subpassDescription.inputAttachmentCount);
				const VkAttachmentReference& attachmentReference = subpassDescription.pInputAttachments[aspectReference.inputAttachmentIndex];
				if(attachmentReference.attachment != VK_ATTACHMENT_UNUSED)
				{
					// If the pNext chain includes an instance of VkRenderPassInputAttachmentAspectCreateInfo, for any
					// element of the pInputAttachments member of any element of pSubpasses where the attachment member
					// is not VK_ATTACHMENT_UNUSED, the aspectMask member of the corresponding element of
					// VkRenderPassInputAttachmentAspectCreateInfo::pAspectReferences must only include aspects that are
					// present in images of the format specified by the element of pAttachments at attachment
					vk::Format format(pCreateInfo->pAttachments[attachmentReference.attachment].format);
					bool isDepth = format.isDepth();
					bool isStencil = format.isStencil();
					ASSERT(!(aspectReference.aspectMask & VK_IMAGE_ASPECT_COLOR_BIT) || (!isDepth && !isStencil));
					ASSERT(!(aspectReference.aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT) || isDepth);
					ASSERT(!(aspectReference.aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT) || isStencil);
				}
			}
		}
		break;
		case VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO:
		{
			const VkRenderPassMultiviewCreateInfo* multiviewCreateInfo = reinterpret_cast<const VkRenderPassMultiviewCreateInfo*>(extensionCreateInfo);
			ASSERT((multiviewCreateInfo->subpassCount == 0) || (multiviewCreateInfo->subpassCount == pCreateInfo->subpassCount));
			ASSERT((multiviewCreateInfo->dependencyCount == 0) || (multiviewCreateInfo->dependencyCount == pCreateInfo->dependencyCount));

			bool zeroMask = (multiviewCreateInfo->pViewMasks[0] == 0);
			for(uint32_t i = 1; i < multiviewCreateInfo->subpassCount; i++)
			{
				ASSERT((multiviewCreateInfo->pViewMasks[i] == 0) == zeroMask);
			}

			if(zeroMask)
			{
				ASSERT(multiviewCreateInfo->correlationMaskCount == 0);
			}

			for(uint32_t i = 0; i < multiviewCreateInfo->dependencyCount; i++)
			{
				const VkSubpassDependency &dependency = pCreateInfo->pDependencies[i];
				if(multiviewCreateInfo->pViewOffsets[i] != 0)
				{
					ASSERT(dependency.srcSubpass != dependency.dstSubpass);
					ASSERT(dependency.dependencyFlags & VK_DEPENDENCY_VIEW_LOCAL_BIT);
				}
				if(zeroMask)
				{
					ASSERT(!(dependency.dependencyFlags & VK_DEPENDENCY_VIEW_LOCAL_BIT));
				}
			}

			// If the pNext chain includes an instance of VkRenderPassMultiviewCreateInfo,
			// each element of its pViewMask member must not include a bit at a position
			// greater than the value of VkPhysicalDeviceLimits::maxFramebufferLayers
			// pViewMask is a 32 bit value. If maxFramebufferLayers > 32, it's impossible
			// for pViewMask to contain a bit at an illegal position
			// Note: Verify pViewMask values instead if we hit this assert
			ASSERT(vk::Cast(vk::Cast(device)->getPhysicalDevice())->getProperties().limits.maxFramebufferLayers >= 32);
		}
		break;
		default:
			UNIMPLEMENTED("extensionCreateInfo->sType");
			break;
		}

		extensionCreateInfo = extensionCreateInfo->pNext;
	}

	return vk::RenderPass::Create(pAllocator, pCreateInfo, pRenderPass);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkRenderPass renderPass = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
		    device, renderPass, pAllocator);

	vk::destroy(renderPass, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL vkGetRenderAreaGranularity(VkDevice device, VkRenderPass renderPass, VkExtent2D* pGranularity)
{
	TRACE("(VkDevice device = 0x%X, VkRenderPass renderPass = 0x%X, VkExtent2D* pGranularity = 0x%X)",
	      device, renderPass, pGranularity);

	vk::Cast(renderPass)->getRenderAreaGranularity(pGranularity);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool)
{
	TRACE("(VkDevice device = 0x%X, const VkCommandPoolCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkCommandPool* pCommandPool = 0x%X)",
		    device, pCreateInfo, pAllocator, pCommandPool);

	if(pCreateInfo->pNext)
	{
		UNIMPLEMENTED("pCreateInfo->pNext");
	}

	return vk::CommandPool::Create(pAllocator, pCreateInfo, pCommandPool);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkCommandPool commandPool = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
		    device, commandPool, pAllocator);

	vk::destroy(commandPool, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags)
{
	TRACE("(VkDevice device = 0x%X, VkCommandPool commandPool = 0x%X, VkCommandPoolResetFlags flags = %d )",
		device, commandPool, flags);

	return vk::Cast(commandPool)->reset(flags);
}

VKAPI_ATTR VkResult VKAPI_CALL vkAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers)
{
	TRACE("(VkDevice device = 0x%X, const VkCommandBufferAllocateInfo* pAllocateInfo = 0x%X, VkCommandBuffer* pCommandBuffers = 0x%X)",
		    device, pAllocateInfo, pCommandBuffers);

	if(pAllocateInfo->pNext)
	{
		UNIMPLEMENTED("pAllocateInfo->pNext");
	}

	return vk::Cast(pAllocateInfo->commandPool)->allocateCommandBuffers(
		pAllocateInfo->level, pAllocateInfo->commandBufferCount, pCommandBuffers);
}

VKAPI_ATTR void VKAPI_CALL vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers)
{
	TRACE("(VkDevice device = 0x%X, VkCommandPool commandPool = 0x%X, uint32_t commandBufferCount = %d, const VkCommandBuffer* pCommandBuffers = 0x%X)",
		    device, commandPool, commandBufferCount, pCommandBuffers);

	vk::Cast(commandPool)->freeCommandBuffers(commandBufferCount, pCommandBuffers);
}

VKAPI_ATTR VkResult VKAPI_CALL vkBeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, const VkCommandBufferBeginInfo* pBeginInfo = 0x%X)",
		    commandBuffer, pBeginInfo);

	if(pBeginInfo->pNext)
	{
		UNIMPLEMENTED("pBeginInfo->pNext");
	}

	return vk::Cast(commandBuffer)->begin(pBeginInfo->flags, pBeginInfo->pInheritanceInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEndCommandBuffer(VkCommandBuffer commandBuffer)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X)", commandBuffer);

	return vk::Cast(commandBuffer)->end();
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags)
{
	TRACE("VkCommandBuffer commandBuffer = 0x%X, VkCommandBufferResetFlags flags = %d", commandBuffer, flags);

	return vk::Cast(commandBuffer)->reset(flags);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkPipelineBindPoint pipelineBindPoint = %d, VkPipeline pipeline = 0x%X)",
		    commandBuffer, pipelineBindPoint, pipeline);

	vk::Cast(commandBuffer)->bindPipeline(pipelineBindPoint, pipeline);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, uint32_t firstViewport = %d, uint32_t viewportCount = %d, const VkViewport* pViewports = 0x%X)",
	      commandBuffer, firstViewport, viewportCount, pViewports);

	vk::Cast(commandBuffer)->setViewport(firstViewport, viewportCount, pViewports);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, uint32_t firstScissor = %d, uint32_t scissorCount = %d, const VkRect2D* pScissors = 0x%X)",
	      commandBuffer, firstScissor, scissorCount, pScissors);

	vk::Cast(commandBuffer)->setScissor(firstScissor, scissorCount, pScissors);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, float lineWidth = %f)", commandBuffer, lineWidth);

	vk::Cast(commandBuffer)->setLineWidth(lineWidth);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBias(VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, float depthBiasConstantFactor = %f, float depthBiasClamp = %f, float depthBiasSlopeFactor = %f)",
	      commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);

	vk::Cast(commandBuffer)->setDepthBias(depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetBlendConstants(VkCommandBuffer commandBuffer, const float blendConstants[4])
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, const float blendConstants[4] = {%f, %f, %f, %f})",
	      commandBuffer, blendConstants[0], blendConstants[1], blendConstants[2], blendConstants[3]);

	vk::Cast(commandBuffer)->setBlendConstants(blendConstants);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBounds(VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, float minDepthBounds = %f, float maxDepthBounds = %f)",
	      commandBuffer, minDepthBounds, maxDepthBounds);

	vk::Cast(commandBuffer)->setDepthBounds(minDepthBounds, maxDepthBounds);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilCompareMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t compareMask)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkStencilFaceFlags faceMask = %d, uint32_t compareMask = %d)",
	      commandBuffer, faceMask, compareMask);

	vk::Cast(commandBuffer)->setStencilCompareMask(faceMask, compareMask);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilWriteMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t writeMask)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkStencilFaceFlags faceMask = %d, uint32_t writeMask = %d)",
	      commandBuffer, faceMask, writeMask);

	vk::Cast(commandBuffer)->setStencilWriteMask(faceMask, writeMask);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilReference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t reference)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkStencilFaceFlags faceMask = %d, uint32_t reference = %d)",
	      commandBuffer, faceMask, reference);

	vk::Cast(commandBuffer)->setStencilReference(faceMask, reference);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkPipelineBindPoint pipelineBindPoint = %d, VkPipelineLayout layout = 0x%X, uint32_t firstSet = %d, uint32_t descriptorSetCount = %d, const VkDescriptorSet* pDescriptorSets = 0x%X, uint32_t dynamicOffsetCount = %d, const uint32_t* pDynamicOffsets = 0x%X)",
	      commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);

	vk::Cast(commandBuffer)->bindDescriptorSets(pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkBuffer buffer = 0x%X, VkDeviceSize offset = %d, VkIndexType indexType = %d)",
	      commandBuffer, buffer, offset, indexType);

	vk::Cast(commandBuffer)->bindIndexBuffer(buffer, offset, indexType);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, uint32_t firstBinding = %d, uint32_t bindingCount = %d, const VkBuffer* pBuffers = 0x%X, const VkDeviceSize* pOffsets = 0x%X)",
		    commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);

	vk::Cast(commandBuffer)->bindVertexBuffers(firstBinding, bindingCount, pBuffers, pOffsets);
}

VKAPI_ATTR void VKAPI_CALL vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, uint32_t vertexCount = %d, uint32_t instanceCount = %d, uint32_t firstVertex = %d, uint32_t firstInstance = %d)",
		    commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);

	vk::Cast(commandBuffer)->draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, uint32_t indexCount = %d, uint32_t instanceCount = %d, uint32_t firstIndex = %d, int32_t vertexOffset = %d, uint32_t firstInstance = %d)",
		    commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);

	vk::Cast(commandBuffer)->drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkBuffer buffer = 0x%X, VkDeviceSize offset = %d, uint32_t drawCount = %d, uint32_t stride = %d)",
		    commandBuffer, buffer, offset, drawCount, stride);

	vk::Cast(commandBuffer)->drawIndirect(buffer, offset, drawCount, stride);
}

VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkBuffer buffer = 0x%X, VkDeviceSize offset = %d, uint32_t drawCount = %d, uint32_t stride = %d)",
		    commandBuffer, buffer, offset, drawCount, stride);

	vk::Cast(commandBuffer)->drawIndexedIndirect(buffer, offset, drawCount, stride);
}

VKAPI_ATTR void VKAPI_CALL vkCmdDispatch(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, uint32_t groupCountX = %d, uint32_t groupCountY = %d, uint32_t groupCountZ = %d)",
	      commandBuffer, groupCountX, groupCountY, groupCountZ);

	vk::Cast(commandBuffer)->dispatch(groupCountX, groupCountY, groupCountZ);
}

VKAPI_ATTR void VKAPI_CALL vkCmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkBuffer buffer = 0x%X, VkDeviceSize offset = %d)",
	      commandBuffer, buffer, offset);

	vk::Cast(commandBuffer)->dispatchIndirect(buffer, offset);
}

VKAPI_ATTR void VKAPI_CALL vkCmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkBuffer srcBuffer = 0x%X, VkBuffer dstBuffer = 0x%X, uint32_t regionCount = %d, const VkBufferCopy* pRegions = 0x%X)",
	      commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);

	vk::Cast(commandBuffer)->copyBuffer(srcBuffer, dstBuffer, regionCount, pRegions);
}

VKAPI_ATTR void VKAPI_CALL vkCmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy* pRegions)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkImage srcImage = 0x%X, VkImageLayout srcImageLayout = %d, VkImage dstImage = 0x%X, VkImageLayout dstImageLayout = %d, uint32_t regionCount = %d, const VkImageCopy* pRegions = 0x%X)",
	      commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);

	vk::Cast(commandBuffer)->copyImage(srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkImage srcImage = 0x%X, VkImageLayout srcImageLayout = %d, VkImage dstImage = 0x%X, VkImageLayout dstImageLayout = %d, uint32_t regionCount = %d, const VkImageBlit* pRegions = 0x%X, VkFilter filter = %d)",
	      commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);

	vk::Cast(commandBuffer)->blitImage(srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
}

VKAPI_ATTR void VKAPI_CALL vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkBuffer srcBuffer = 0x%X, VkImage dstImage = 0x%X, VkImageLayout dstImageLayout = %d, uint32_t regionCount = %d, const VkBufferImageCopy* pRegions = 0x%X)",
	      commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);

	vk::Cast(commandBuffer)->copyBufferToImage(srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
}

VKAPI_ATTR void VKAPI_CALL vkCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy* pRegions)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkImage srcImage = 0x%X, VkImageLayout srcImageLayout = %d, VkBuffer dstBuffer = 0x%X, uint32_t regionCount = %d, const VkBufferImageCopy* pRegions = 0x%X)",
		    commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);

	vk::Cast(commandBuffer)->copyImageToBuffer(srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
}

VKAPI_ATTR void VKAPI_CALL vkCmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkBuffer dstBuffer = 0x%X, VkDeviceSize dstOffset = %d, VkDeviceSize dataSize = %d, const void* pData = 0x%X)",
	      commandBuffer, dstBuffer, dstOffset, dataSize, pData);

	vk::Cast(commandBuffer)->updateBuffer(dstBuffer, dstOffset, dataSize, pData);
}

VKAPI_ATTR void VKAPI_CALL vkCmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkBuffer dstBuffer = 0x%X, VkDeviceSize dstOffset = %d, VkDeviceSize size = %d, uint32_t data = %d)",
	      commandBuffer, dstBuffer, dstOffset, size, data);

	vk::Cast(commandBuffer)->fillBuffer(dstBuffer, dstOffset, size, data);
}

VKAPI_ATTR void VKAPI_CALL vkCmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint32_t rangeCount, const VkImageSubresourceRange* pRanges)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkImage image = 0x%X, VkImageLayout imageLayout = %d, const VkClearColorValue* pColor = 0x%X, uint32_t rangeCount = %d, const VkImageSubresourceRange* pRanges = 0x%X)",
	      commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);

	vk::Cast(commandBuffer)->clearColorImage(image, imageLayout, pColor, rangeCount, pRanges);
}

VKAPI_ATTR void VKAPI_CALL vkCmdClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange* pRanges)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkImage image = 0x%X, VkImageLayout imageLayout = %d, const VkClearDepthStencilValue* pDepthStencil = 0x%X, uint32_t rangeCount = %d, const VkImageSubresourceRange* pRanges = 0x%X)",
	      commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);

	vk::Cast(commandBuffer)->clearDepthStencilImage(image, imageLayout, pDepthStencil, rangeCount, pRanges);
}

VKAPI_ATTR void VKAPI_CALL vkCmdClearAttachments(VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkClearAttachment* pAttachments, uint32_t rectCount, const VkClearRect* pRects)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, uint32_t attachmentCount = %d, const VkClearAttachment* pAttachments = 0x%X, uint32_t rectCount = %d, const VkClearRect* pRects = 0x%X)",
	      commandBuffer, attachmentCount, pAttachments, rectCount,  pRects);

	vk::Cast(commandBuffer)->clearAttachments(attachmentCount, pAttachments, rectCount, pRects);
}

VKAPI_ATTR void VKAPI_CALL vkCmdResolveImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve* pRegions)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkImage srcImage = 0x%X, VkImageLayout srcImageLayout = %d, VkImage dstImage = 0x%X, VkImageLayout dstImageLayout = %d, uint32_t regionCount = %d, const VkImageResolve* pRegions = 0x%X)",
	      commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);

	vk::Cast(commandBuffer)->resolveImage(srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkEvent event = 0x%X, VkPipelineStageFlags stageMask = %d)",
	      commandBuffer, event, stageMask);

	vk::Cast(commandBuffer)->setEvent(event, stageMask);
}

VKAPI_ATTR void VKAPI_CALL vkCmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkEvent event = 0x%X, VkPipelineStageFlags stageMask = %d)",
	      commandBuffer, event, stageMask);

	vk::Cast(commandBuffer)->resetEvent(event, stageMask);
}

VKAPI_ATTR void VKAPI_CALL vkCmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, uint32_t eventCount = %d, const VkEvent* pEvents = 0x%X, VkPipelineStageFlags srcStageMask = %d, VkPipelineStageFlags dstStageMask = %d, uint32_t memoryBarrierCount = %d, const VkMemoryBarrier* pMemoryBarriers = 0x%X, uint32_t bufferMemoryBarrierCount = %d, const VkBufferMemoryBarrier* pBufferMemoryBarriers = 0x%X, uint32_t imageMemoryBarrierCount = %d, const VkImageMemoryBarrier* pImageMemoryBarriers = 0x%X)",
	      commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);

	vk::Cast(commandBuffer)->waitEvents(eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

VKAPI_ATTR void VKAPI_CALL vkCmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkPipelineStageFlags srcStageMask = 0x%X, VkPipelineStageFlags dstStageMask = 0x%X, VkDependencyFlags dependencyFlags = %d, uint32_t memoryBarrierCount = %d, onst VkMemoryBarrier* pMemoryBarriers = 0x%X,"
	      " uint32_t bufferMemoryBarrierCount = %d, const VkBufferMemoryBarrier* pBufferMemoryBarriers = 0x%X, uint32_t imageMemoryBarrierCount = %d, const VkImageMemoryBarrier* pImageMemoryBarriers = 0x%X)",
	      commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);

	vk::Cast(commandBuffer)->pipelineBarrier(srcStageMask, dstStageMask, dependencyFlags,
	                                         memoryBarrierCount, pMemoryBarriers,
	                                         bufferMemoryBarrierCount, pBufferMemoryBarriers,
	                                         imageMemoryBarrierCount, pImageMemoryBarriers);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkQueryPool queryPool = 0x%X, uint32_t query = %d, VkQueryControlFlags flags = %d)",
	      commandBuffer, queryPool, query, flags);

	vk::Cast(commandBuffer)->beginQuery(queryPool, query, flags);
}

VKAPI_ATTR void VKAPI_CALL vkCmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkQueryPool queryPool = 0x%X, uint32_t query = %d)",
	      commandBuffer, queryPool, query);

	vk::Cast(commandBuffer)->endQuery(queryPool, query);
}

VKAPI_ATTR void VKAPI_CALL vkCmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkQueryPool queryPool = 0x%X, uint32_t firstQuery = %d, uint32_t queryCount = %d)",
	      commandBuffer, queryPool, firstQuery, queryCount);

	vk::Cast(commandBuffer)->resetQueryPool(queryPool, firstQuery, queryCount);
}

VKAPI_ATTR void VKAPI_CALL vkCmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkPipelineStageFlagBits pipelineStage = %d, VkQueryPool queryPool = 0x%X, uint32_t query = %d)",
	      commandBuffer, pipelineStage, queryPool, query);

	vk::Cast(commandBuffer)->writeTimestamp(pipelineStage, queryPool, query);
}

VKAPI_ATTR void VKAPI_CALL vkCmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkQueryPool queryPool = 0x%X, uint32_t firstQuery = %d, uint32_t queryCount = %d, VkBuffer dstBuffer = 0x%X, VkDeviceSize dstOffset = %d, VkDeviceSize stride = %d, VkQueryResultFlags flags = %d)",
	      commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);

	vk::Cast(commandBuffer)->copyQueryPoolResults(queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
}

VKAPI_ATTR void VKAPI_CALL vkCmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkPipelineLayout layout = 0x%X, VkShaderStageFlags stageFlags = %d, uint32_t offset = %d, uint32_t size = %d, const void* pValues = 0x%X)",
	      commandBuffer, layout, stageFlags, offset, size, pValues);

	vk::Cast(commandBuffer)->pushConstants(layout, stageFlags, offset, size, pValues);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, const VkRenderPassBeginInfo* pRenderPassBegin = 0x%X, VkSubpassContents contents = %d)",
	      commandBuffer, pRenderPassBegin, contents);

	if(pRenderPassBegin->pNext)
	{
		UNIMPLEMENTED("pRenderPassBegin->pNext");
	}

	vk::Cast(commandBuffer)->beginRenderPass(pRenderPassBegin->renderPass, pRenderPassBegin->framebuffer,
	                                         pRenderPassBegin->renderArea, pRenderPassBegin->clearValueCount,
	                                         pRenderPassBegin->pClearValues, contents);
}

VKAPI_ATTR void VKAPI_CALL vkCmdNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents contents)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, VkSubpassContents contents = %d)",
	      commandBuffer, contents);

	vk::Cast(commandBuffer)->nextSubpass(contents);
}

VKAPI_ATTR void VKAPI_CALL vkCmdEndRenderPass(VkCommandBuffer commandBuffer)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X)", commandBuffer);

	vk::Cast(commandBuffer)->endRenderPass();
}

VKAPI_ATTR void VKAPI_CALL vkCmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers)
{
	TRACE("(VkCommandBuffer commandBuffer = 0x%X, uint32_t commandBufferCount = %d, const VkCommandBuffer* pCommandBuffers = 0x%X)",
	      commandBuffer, commandBufferCount, pCommandBuffers);

	vk::Cast(commandBuffer)->executeCommands(commandBufferCount, pCommandBuffers);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceVersion(uint32_t* pApiVersion)
{
	TRACE("(uint32_t* pApiVersion = 0x%X)", pApiVersion);
	*pApiVersion = vk::API_VERSION;
	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkBindBufferMemory2(VkDevice device, uint32_t bindInfoCount, const VkBindBufferMemoryInfo* pBindInfos)
{
	TRACE("(VkDevice device = 0x%X, uint32_t bindInfoCount = %d, const VkBindBufferMemoryInfo* pBindInfos = 0x%X)",
	      device, bindInfoCount, pBindInfos);

	for(uint32_t i = 0; i < bindInfoCount; i++)
	{
		if(pBindInfos[i].pNext)
		{
			UNIMPLEMENTED("pBindInfos[%d].pNext", i);
		}

		vk::Cast(pBindInfos[i].buffer)->bind(pBindInfos[i].memory, pBindInfos[i].memoryOffset);
	}

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkBindImageMemory2(VkDevice device, uint32_t bindInfoCount, const VkBindImageMemoryInfo* pBindInfos)
{
	TRACE("()");
	UNIMPLEMENTED("vkBindImageMemory2");
	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetDeviceGroupPeerMemoryFeatures(VkDevice device, uint32_t heapIndex, uint32_t localDeviceIndex, uint32_t remoteDeviceIndex, VkPeerMemoryFeatureFlags* pPeerMemoryFeatures)
{
	TRACE("(VkDevice device = 0x%X, uint32_t heapIndex = %d, uint32_t localDeviceIndex = %d, uint32_t remoteDeviceIndex = %d, VkPeerMemoryFeatureFlags* pPeerMemoryFeatures = 0x%X)",
	      device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);

	ASSERT(localDeviceIndex != remoteDeviceIndex); // "localDeviceIndex must not equal remoteDeviceIndex"
	UNREACHABLE("remoteDeviceIndex: %d", int(remoteDeviceIndex));   // Only one physical device is supported, and since the device indexes can't be equal, this should never be called.
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetDeviceMask(VkCommandBuffer commandBuffer, uint32_t deviceMask)
{
	TRACE("()");
	UNIMPLEMENTED("vkCmdSetDeviceMask");
}

VKAPI_ATTR void VKAPI_CALL vkCmdDispatchBase(VkCommandBuffer commandBuffer, uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	TRACE("()");
	UNIMPLEMENTED("vkCmdDispatchBase");
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDeviceGroups(VkInstance instance, uint32_t* pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties)
{
	TRACE("VkInstance instance = 0x%X, uint32_t* pPhysicalDeviceGroupCount = 0x%X, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties = 0x%X",
	      instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);

	if(!pPhysicalDeviceGroupProperties)
	{
		*pPhysicalDeviceGroupCount = vk::Cast(instance)->getPhysicalDeviceGroupCount();
	}
	else
	{
		vk::Cast(instance)->getPhysicalDeviceGroups(*pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
	}

	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetImageMemoryRequirements2(VkDevice device, const VkImageMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements)
{
	TRACE("(VkDevice device = 0x%X, const VkImageMemoryRequirementsInfo2* pInfo = 0x%X, VkMemoryRequirements2* pMemoryRequirements = 0x%X)",
	      device, pInfo, pMemoryRequirements);

	if(pInfo->pNext)
	{
		UNIMPLEMENTED("pInfo->pNext");
	}

	VkBaseOutStructure* extensionRequirements = reinterpret_cast<VkBaseOutStructure*>(pMemoryRequirements->pNext);
	while(extensionRequirements)
	{
		switch(extensionRequirements->sType)
		{
		case VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS:
		{
			auto& requirements = *reinterpret_cast<VkMemoryDedicatedRequirements*>(extensionRequirements);
			requirements.prefersDedicatedAllocation = VK_FALSE;
			requirements.requiresDedicatedAllocation = VK_FALSE;
		}
		break;
		default:
			UNIMPLEMENTED("extensionRequirements->sType");
			break;
		}

		extensionRequirements = extensionRequirements->pNext;
	}

	vkGetImageMemoryRequirements(device, pInfo->image, &(pMemoryRequirements->memoryRequirements));
}

VKAPI_ATTR void VKAPI_CALL vkGetBufferMemoryRequirements2(VkDevice device, const VkBufferMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements)
{
	TRACE("(VkDevice device = 0x%X, const VkBufferMemoryRequirementsInfo2* pInfo = 0x%X, VkMemoryRequirements2* pMemoryRequirements = 0x%X)",
	      device, pInfo, pMemoryRequirements);

	if(pInfo->pNext)
	{
		UNIMPLEMENTED("pInfo->pNext");
	}

	VkBaseOutStructure* extensionRequirements = reinterpret_cast<VkBaseOutStructure*>(pMemoryRequirements->pNext);
	while(extensionRequirements)
	{
		switch(extensionRequirements->sType)
		{
		case VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS:
			{
				auto& requirements = *reinterpret_cast<VkMemoryDedicatedRequirements*>(extensionRequirements);
				requirements.prefersDedicatedAllocation = VK_FALSE;
				requirements.requiresDedicatedAllocation = VK_FALSE;
			}
			break;
		default:
			UNIMPLEMENTED("extensionRequirements->sType");
			break;
		}

		extensionRequirements = extensionRequirements->pNext;
	}

	vkGetBufferMemoryRequirements(device, pInfo->buffer, &(pMemoryRequirements->memoryRequirements));
}

VKAPI_ATTR void VKAPI_CALL vkGetImageSparseMemoryRequirements2(VkDevice device, const VkImageSparseMemoryRequirementsInfo2* pInfo, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements2* pSparseMemoryRequirements)
{
	TRACE("(VkDevice device = 0x%X, const VkImageSparseMemoryRequirementsInfo2* pInfo = 0x%X, uint32_t* pSparseMemoryRequirementCount = 0x%X, VkSparseImageMemoryRequirements2* pSparseMemoryRequirements = 0x%X)",
	      device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);

	if(pInfo->pNext || pSparseMemoryRequirements->pNext)
	{
		UNIMPLEMENTED("pInfo->pNext || pSparseMemoryRequirements->pNext");
	}

	vkGetImageSparseMemoryRequirements(device, pInfo->image, pSparseMemoryRequirementCount, &(pSparseMemoryRequirements->memoryRequirements));
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2* pFeatures)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, VkPhysicalDeviceFeatures2* pFeatures = 0x%X)", physicalDevice, pFeatures);

	VkBaseOutStructure* extensionFeatures = reinterpret_cast<VkBaseOutStructure*>(pFeatures->pNext);
	while(extensionFeatures)
	{
		switch(extensionFeatures->sType)
		{
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES:
			{
				auto& features = *reinterpret_cast<VkPhysicalDeviceSamplerYcbcrConversionFeatures*>(extensionFeatures);
				vk::Cast(physicalDevice)->getFeatures(&features);
			}
			break;
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES:
			{
				auto& features = *reinterpret_cast<VkPhysicalDevice16BitStorageFeatures*>(extensionFeatures);
				vk::Cast(physicalDevice)->getFeatures(&features);
			}
			break;
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES:
			{
				auto& features = *reinterpret_cast<VkPhysicalDeviceVariablePointerFeatures*>(extensionFeatures);
				vk::Cast(physicalDevice)->getFeatures(&features);
			}
			break;
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR:
			{
				auto& features = *reinterpret_cast<VkPhysicalDevice8BitStorageFeaturesKHR*>(extensionFeatures);
				vk::Cast(physicalDevice)->getFeatures(&features);
			}
			break;
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES:
			{
				auto& features = *reinterpret_cast<VkPhysicalDeviceMultiviewFeatures*>(extensionFeatures);
				vk::Cast(physicalDevice)->getFeatures(&features);
			}
			break;
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES:
			{
				auto& features = *reinterpret_cast<VkPhysicalDeviceProtectedMemoryFeatures*>(extensionFeatures);
				vk::Cast(physicalDevice)->getFeatures(&features);
			}
			break;
		default:
			// "the [driver] must skip over, without processing (other than reading the sType and pNext members) any structures in the chain with sType values not defined by [supported extenions]"
			UNIMPLEMENTED("extensionFeatures->sType");   // TODO(b/119321052): UNIMPLEMENTED() should be used only for features that must still be implemented. Use a more informational macro here.
			break;
		}

		extensionFeatures = extensionFeatures->pNext;
	}

	vkGetPhysicalDeviceFeatures(physicalDevice, &(pFeatures->features));
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties2* pProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, VkPhysicalDeviceProperties2* pProperties = 0x%X)", physicalDevice, pProperties);

	VkBaseOutStructure* extensionProperties = reinterpret_cast<VkBaseOutStructure*>(pProperties->pNext);
	while(extensionProperties)
	{
		switch(extensionProperties->sType)
		{
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES:
			{
				auto& properties = *reinterpret_cast<VkPhysicalDeviceIDProperties*>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(&properties);
			}
			break;
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES:
			{
				auto& properties = *reinterpret_cast<VkPhysicalDeviceMaintenance3Properties*>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(&properties);
			}
			break;
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES:
			{
				auto& properties = *reinterpret_cast<VkPhysicalDeviceMultiviewProperties*>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(&properties);
			}
			break;
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES:
			{
				auto& properties = *reinterpret_cast<VkPhysicalDevicePointClippingProperties*>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(&properties);
			}
			break;
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_PROPERTIES:
			{
				auto& properties = *reinterpret_cast<VkPhysicalDeviceProtectedMemoryProperties*>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(&properties);
			}
			break;
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES:
			{
				auto& properties = *reinterpret_cast<VkPhysicalDeviceSubgroupProperties*>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(&properties);
			}
			break;
		case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT:
			// Explicitly ignored, since VK_EXT_sample_locations is not supported
			ASSERT(!HasExtensionProperty(VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME, deviceExtensionProperties,
			                             sizeof(deviceExtensionProperties) / sizeof(deviceExtensionProperties[0])));
			break;
		default:
			// "the [driver] must skip over, without processing (other than reading the sType and pNext members) any structures in the chain with sType values not defined by [supported extenions]"
			UNIMPLEMENTED("extensionProperties->sType");   // TODO(b/119321052): UNIMPLEMENTED() should be used only for features that must still be implemented. Use a more informational macro here.
			break;
		}

		extensionProperties = extensionProperties->pNext;
	}

	vkGetPhysicalDeviceProperties(physicalDevice, &(pProperties->properties));
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFormatProperties2(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties2* pFormatProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, VkFormat format = %d, VkFormatProperties2* pFormatProperties = 0x%X)",
		    physicalDevice, format, pFormatProperties);

	if(pFormatProperties->pNext)
	{
		UNIMPLEMENTED("pFormatProperties->pNext");
	}

	vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &(pFormatProperties->formatProperties));
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceImageFormatProperties2(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo, VkImageFormatProperties2* pImageFormatProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo = 0x%X, VkImageFormatProperties2* pImageFormatProperties = 0x%X)",
		    physicalDevice, pImageFormatInfo, pImageFormatProperties);

	if(pImageFormatInfo->pNext || pImageFormatProperties->pNext)
	{
		UNIMPLEMENTED("pImageFormatInfo->pNext || pImageFormatProperties->pNext");
	}

	return vkGetPhysicalDeviceImageFormatProperties(physicalDevice,
		                                            pImageFormatInfo->format,
		                                            pImageFormatInfo->type,
		                                            pImageFormatInfo->tiling,
		                                            pImageFormatInfo->usage,
		                                            pImageFormatInfo->flags,
		                                            &(pImageFormatProperties->imageFormatProperties));
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties2(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties2* pQueueFamilyProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, uint32_t* pQueueFamilyPropertyCount = 0x%X, VkQueueFamilyProperties2* pQueueFamilyProperties = 0x%X)",
		physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);

	if(pQueueFamilyProperties && pQueueFamilyProperties->pNext)
	{
		UNIMPLEMENTED("pQueueFamilyProperties->pNext");
	}

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount,
		pQueueFamilyProperties ? &(pQueueFamilyProperties->queueFamilyProperties) : nullptr);
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceMemoryProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2* pMemoryProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, VkPhysicalDeviceMemoryProperties2* pMemoryProperties = 0x%X)", physicalDevice, pMemoryProperties);

	if(pMemoryProperties->pNext)
	{
		UNIMPLEMENTED("pMemoryProperties->pNext");
	}

	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &(pMemoryProperties->memoryProperties));
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceSparseImageFormatProperties2(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo, uint32_t* pPropertyCount, VkSparseImageFormatProperties2* pProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo = 0x%X, uint32_t* pPropertyCount = 0x%X, VkSparseImageFormatProperties2* pProperties = 0x%X)",
	     physicalDevice, pFormatInfo, pPropertyCount, pProperties);

	if(pProperties && pProperties->pNext)
	{
		UNIMPLEMENTED("pProperties->pNext");
	}

	vkGetPhysicalDeviceSparseImageFormatProperties(physicalDevice, pFormatInfo->format, pFormatInfo->type,
	                                               pFormatInfo->samples, pFormatInfo->usage, pFormatInfo->tiling,
	                                               pPropertyCount, pProperties ? &(pProperties->properties) : nullptr);
}

VKAPI_ATTR void VKAPI_CALL vkTrimCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolTrimFlags flags)
{
	TRACE("(VkDevice device = 0x%X, VkCommandPool commandPool = 0x%X, VkCommandPoolTrimFlags flags = %d)",
	      device, commandPool, flags);

	vk::Cast(commandPool)->trim(flags);
}

VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue2(VkDevice device, const VkDeviceQueueInfo2* pQueueInfo, VkQueue* pQueue)
{
	TRACE("(VkDevice device = 0x%X, const VkDeviceQueueInfo2* pQueueInfo = 0x%X, VkQueue* pQueue = 0x%X)",
	      device, pQueueInfo, pQueue);

	if(pQueueInfo->pNext)
	{
		UNIMPLEMENTED("pQueueInfo->pNext");
	}

	// The only flag that can be set here is VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT
	// According to the Vulkan spec, 4.3.1. Queue Family Properties:
	// "VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT specifies that the device queue is a
	//  protected-capable queue. If the protected memory feature is not enabled,
	//  the VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT bit of flags must not be set."
	if(pQueueInfo->flags)
	{
		*pQueue = VK_NULL_HANDLE;
	}
	else
	{
		vkGetDeviceQueue(device, pQueueInfo->queueFamilyIndex, pQueueInfo->queueIndex, pQueue);
	}
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateSamplerYcbcrConversion(VkDevice device, const VkSamplerYcbcrConversionCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSamplerYcbcrConversion* pYcbcrConversion)
{
	TRACE("()");
	UNIMPLEMENTED("vkCreateSamplerYcbcrConversion");
	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroySamplerYcbcrConversion(VkDevice device, VkSamplerYcbcrConversion ycbcrConversion, const VkAllocationCallbacks* pAllocator)
{
	TRACE("()");
	UNIMPLEMENTED("vkDestroySamplerYcbcrConversion");
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorUpdateTemplate(VkDevice device, const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate)
{
	TRACE("(VkDevice device = 0x%X, const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate = 0x%X)",
	      device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);

	if(pCreateInfo->pNext || pCreateInfo->flags || (pCreateInfo->templateType != VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET))
	{
		UNIMPLEMENTED("pCreateInfo->pNext || pCreateInfo->flags || (pCreateInfo->templateType != VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET)");
	}

	return vk::DescriptorUpdateTemplate::Create(pAllocator, pCreateInfo, pDescriptorUpdateTemplate);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorUpdateTemplate(VkDevice device, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkDescriptorUpdateTemplate descriptorUpdateTemplate = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
	      device, descriptorUpdateTemplate, pAllocator);

	vk::destroy(descriptorUpdateTemplate, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL vkUpdateDescriptorSetWithTemplate(VkDevice device, VkDescriptorSet descriptorSet, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const void* pData)
{
	TRACE("(VkDevice device = 0x%X, VkDescriptorSet descriptorSet = 0x%X, VkDescriptorUpdateTemplate descriptorUpdateTemplate = 0x%X, const void* pData = 0x%X)",
	      device, descriptorSet, descriptorUpdateTemplate, pData);

	vk::Cast(descriptorUpdateTemplate)->updateDescriptorSet(descriptorSet, pData);
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalBufferProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo, VkExternalBufferProperties* pExternalBufferProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo = 0x%X, VkExternalBufferProperties* pExternalBufferProperties = 0x%X)",
	      physicalDevice, pExternalBufferInfo, pExternalBufferProperties);

	UNIMPLEMENTED("vkGetPhysicalDeviceExternalBufferProperties");
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalFenceProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo, VkExternalFenceProperties* pExternalFenceProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo = 0x%X, VkExternalFenceProperties* pExternalFenceProperties = 0x%X)",
	      physicalDevice, pExternalFenceInfo, pExternalFenceProperties);

	UNIMPLEMENTED("vkGetPhysicalDeviceExternalFenceProperties");
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalSemaphoreProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo, VkExternalSemaphoreProperties* pExternalSemaphoreProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo = 0x%X, VkExternalSemaphoreProperties* pExternalSemaphoreProperties = 0x%X)",
	      physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);

	UNIMPLEMENTED("vkGetPhysicalDeviceExternalSemaphoreProperties");
}

VKAPI_ATTR void VKAPI_CALL vkGetDescriptorSetLayoutSupport(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, VkDescriptorSetLayoutSupport* pSupport)
{
	TRACE("(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, VkDescriptorSetLayoutSupport* pSupport)",
	      device, pCreateInfo, pSupport);

	vk::Cast(device)->getDescriptorSetLayoutSupport(pCreateInfo, pSupport);
}

#ifdef VK_USE_PLATFORM_XLIB_KHR
VKAPI_ATTR VkResult VKAPI_CALL vkCreateXlibSurfaceKHR(VkInstance instance, const VkXlibSurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface)
{
	TRACE("(VkInstance instance = 0x%X, VkXlibSurfaceCreateInfoKHR* pCreateInfo = 0x%X, VkAllocationCallbacks* pAllocator = 0x%X, VkSurface* pSurface = 0x%X)",
			instance, pCreateInfo, pAllocator, pSurface);

	return vk::XlibSurfaceKHR::Create(pAllocator, pCreateInfo, pSurface);
}
#endif

#ifndef __ANDROID__
VKAPI_ATTR void VKAPI_CALL vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks* pAllocator)
{
    TRACE("(VkInstance instance = 0x%X, VkSurfaceKHR surface = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
            instance, surface, pAllocator);

    vk::destroy(surface, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32* pSupported)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, uint32_t queueFamilyIndex = 0x%X, VkSurface surface = 0x%X, VKBool32* pSupported = 0x%X)",
			physicalDevice, queueFamilyIndex, surface, pSupported);

	*pSupported =  VK_TRUE;
	return VK_SUCCESS;
}


VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, VkSurfaceKHR surface = 0x%X, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities = 0x%X)",
			physicalDevice, surface, pSurfaceCapabilities);

	vk::Cast(surface)->getSurfaceCapabilities(pSurfaceCapabilities);
	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, VkSurfaceKHR surface = 0x%X. uint32_t* pSurfaceFormatCount = 0x%X, VkSurfaceFormatKHR* pSurfaceFormats)",
			physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);

	if(!pSurfaceFormats)
	{
		*pSurfaceFormatCount = vk::Cast(surface)->getSurfaceFormatsCount();
		return VK_SUCCESS;
	}

	return vk::Cast(surface)->getSurfaceFormats(pSurfaceFormatCount, pSurfaceFormats);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes)
{
	TRACE("(VkPhysicalDevice physicalDevice = 0x%X, VkSurfaceKHR surface = 0x%X uint32_t* pPresentModeCount = 0x%X, VkPresentModeKHR* pPresentModes = 0x%X)",
			physicalDevice, surface, pPresentModeCount, pPresentModes);

	if(!pPresentModes)
	{
		*pPresentModeCount = vk::Cast(surface)->getPresentModeCount();
		return VK_SUCCESS;
	}

	return vk::Cast(surface)->getPresentModes(pPresentModeCount, pPresentModes);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain)
{
	TRACE("(VkDevice device = 0x%X, const VkSwapchainCreateInfoKHR* pCreateInfo = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X, VkSwapchainKHR* pSwapchain = 0x%X)",
			device, pCreateInfo, pAllocator, pSwapchain);

	if(pCreateInfo->oldSwapchain)
	{
		vk::Cast(pCreateInfo->oldSwapchain)->retire();
	}

	if(vk::Cast(pCreateInfo->surface)->getAssociatedSwapchain() != VK_NULL_HANDLE)
	{
		return VK_ERROR_NATIVE_WINDOW_IN_USE_KHR;
	}

	VkResult status = vk::SwapchainKHR::Create(pAllocator, pCreateInfo, pSwapchain);

	if(status != VK_SUCCESS)
	{
		return status;
	}

	status = vk::Cast(*pSwapchain)->createImages(device);

	if(status != VK_SUCCESS)
	{
		vk::destroy(*pSwapchain, pAllocator);
		return status;
	}

	vk::Cast(pCreateInfo->surface)->associateSwapchain(*pSwapchain);

	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator)
{
	TRACE("(VkDevice device = 0x%X, VkSwapchainKHR swapchain = 0x%X, const VkAllocationCallbacks* pAllocator = 0x%X)",
			device, swapchain, pAllocator);

	vk::destroy(swapchain, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain, uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages)
{
	TRACE("(VkDevice device = 0x%X, VkSwapchainKHR swapchain = 0x%X, uint32_t* pSwapchainImageCount = 0x%X, VkImage* pSwapchainImages = 0x%X)",
			device, swapchain, pSwapchainImageCount, pSwapchainImages);

	if(!pSwapchainImages)
	{
		*pSwapchainImageCount = vk::Cast(swapchain)->getImageCount();
		return VK_SUCCESS;
	}

	return vk::Cast(swapchain)->getImages(pSwapchainImageCount, pSwapchainImages);
}

VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex)
{
	TRACE("(VkDevice device = 0x%X, VkSwapchainKHR swapchain = 0x%X, uint64_t timeout = 0x%X, VkSemaphore semaphore = 0x%X, VkFence fence = 0x%X, uint32_t* pImageIndex = 0x%X)",
			device, swapchain, timeout, semaphore, fence, pImageIndex);

	return vk::Cast(swapchain)->getNextImage(timeout, semaphore, fence, pImageIndex);
}

VKAPI_ATTR VkResult VKAPI_CALL vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo)
{
	TRACE("(VkQueue queue = 0x%X, const VkPresentInfoKHR* pPresentInfo = 0x%X)",
			queue, pPresentInfo);

	vk::Cast(queue)->present(pPresentInfo);

	return VK_SUCCESS;
}
#endif    // ! __ANDROID__

}
