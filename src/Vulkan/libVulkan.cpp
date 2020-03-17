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
#include "VkRenderPass.hpp"
#include "VkSampler.hpp"
#include "VkSemaphore.hpp"
#include "VkShaderModule.hpp"
#include "VkStringify.hpp"

#include "System/Debug.hpp"

#if defined(VK_USE_PLATFORM_METAL_EXT) || defined(VK_USE_PLATFORM_MACOS_MVK)
#	include "WSI/MetalSurface.h"
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
#	include "WSI/XcbSurfaceKHR.hpp"
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
#	include "WSI/XlibSurfaceKHR.hpp"
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
#	include "WSI/Win32SurfaceKHR.hpp"
#endif

#ifdef __ANDROID__
#	include "commit.h"
#	include "System/GrallocAndroid.hpp"
#	include <android/log.h>
#	include <sync/sync.h>
#endif

#include "WSI/VkSwapchainKHR.hpp"

#include "Reactor/Nucleus.hpp"

#include "marl/scheduler.h"
#include "marl/thread.h"

#include "System/CPUID.hpp"

#include <algorithm>
#include <cstring>
#include <map>
#include <string>

namespace {

// Enable commit_id.py and #include commit.h for other platforms.
#if defined(__ANDROID__) && defined(ENABLE_BUILD_VERSION_OUTPUT)
void logBuildVersionInformation()
{
	// TODO(b/144093703): Don't call __android_log_print() directly
	__android_log_print(ANDROID_LOG_INFO, "SwiftShader", "SwiftShader Version: %s", SWIFTSHADER_VERSION_STRING);
}
#endif  // __ANDROID__ && ENABLE_BUILD_VERSION_OUTPUT

bool HasExtensionProperty(const char *extensionName, const VkExtensionProperties *extensionProperties, uint32_t extensionPropertiesCount)
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

// setReactorDefaultConfig() sets the default configuration for Vulkan's use of
// Reactor.
void setReactorDefaultConfig()
{
	auto cfg = rr::Config::Edit()
	               .set(rr::Optimization::Level::Default)
	               .clearOptimizationPasses()
	               .add(rr::Optimization::Pass::ScalarReplAggregates)
	               .add(rr::Optimization::Pass::SCCP)
	               .add(rr::Optimization::Pass::CFGSimplification)
	               .add(rr::Optimization::Pass::EarlyCSEPass)
	               .add(rr::Optimization::Pass::CFGSimplification)
	               .add(rr::Optimization::Pass::InstructionCombining);

	rr::Nucleus::adjustDefaultConfig(cfg);
}

void setCPUDefaults()
{
	sw::CPUID::setEnableSSE4_1(true);
	sw::CPUID::setEnableSSSE3(true);
	sw::CPUID::setEnableSSE3(true);
	sw::CPUID::setEnableSSE2(true);
	sw::CPUID::setEnableSSE(true);
}

std::shared_ptr<marl::Scheduler> getOrCreateScheduler()
{
	static std::mutex mutex;
	static std::weak_ptr<marl::Scheduler> schedulerWeak;
	std::unique_lock<std::mutex> lock(mutex);
	auto scheduler = schedulerWeak.lock();
	if(!scheduler)
	{
		scheduler = std::make_shared<marl::Scheduler>();
		scheduler->setThreadInitializer([] {
			sw::CPUID::setFlushToZero(true);
			sw::CPUID::setDenormalsAreZero(true);
		});
		scheduler->setWorkerThreadCount(std::min<size_t>(marl::Thread::numLogicalCPUs(), 16));
		schedulerWeak = scheduler;
	}
	return scheduler;
}

// initializeLibrary() is called by vkCreateInstance() to perform one-off global
// initialization of the swiftshader driver.
void initializeLibrary()
{
	static bool doOnce = [] {
#if defined(__ANDROID__) && defined(ENABLE_BUILD_VERSION_OUTPUT)
		logBuildVersionInformation();
#endif  // __ANDROID__ && ENABLE_BUILD_VERSION_OUTPUT
		setReactorDefaultConfig();
		setCPUDefaults();
		return true;
	}();
	(void)doOnce;
}

template<class T>
void ValidateRenderPassPNextChain(VkDevice device, const T *pCreateInfo)
{
	const VkBaseInStructure *extensionCreateInfo = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext);

	while(extensionCreateInfo)
	{
		switch(extensionCreateInfo->sType)
		{
			case VK_STRUCTURE_TYPE_RENDER_PASS_INPUT_ATTACHMENT_ASPECT_CREATE_INFO:
			{
				const VkRenderPassInputAttachmentAspectCreateInfo *inputAttachmentAspectCreateInfo = reinterpret_cast<const VkRenderPassInputAttachmentAspectCreateInfo *>(extensionCreateInfo);

				for(uint32_t i = 0; i < inputAttachmentAspectCreateInfo->aspectReferenceCount; i++)
				{
					const auto &aspectReference = inputAttachmentAspectCreateInfo->pAspectReferences[i];
					ASSERT(aspectReference.subpass < pCreateInfo->subpassCount);
					const auto &subpassDescription = pCreateInfo->pSubpasses[aspectReference.subpass];
					ASSERT(aspectReference.inputAttachmentIndex < subpassDescription.inputAttachmentCount);
					const auto &attachmentReference = subpassDescription.pInputAttachments[aspectReference.inputAttachmentIndex];
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
				const VkRenderPassMultiviewCreateInfo *multiviewCreateInfo = reinterpret_cast<const VkRenderPassMultiviewCreateInfo *>(extensionCreateInfo);
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
					const auto &dependency = pCreateInfo->pDependencies[i];
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
				ASSERT(vk::Cast(device)->getPhysicalDevice()->getProperties().limits.maxFramebufferLayers >= 32);
			}
			break;
			default:
				LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(extensionCreateInfo->sType).c_str());
				break;
		}

		extensionCreateInfo = extensionCreateInfo->pNext;
	}
}

}  // namespace

extern "C" {
VK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vk_icdGetInstanceProcAddr(VkInstance instance, const char *pName)
{
	TRACE("(VkInstance instance = %p, const char* pName = %p)", instance, pName);

	return vk::GetInstanceProcAddr(vk::Cast(instance), pName);
}

VK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t *pSupportedVersion)
{
	*pSupportedVersion = 3;
	return VK_SUCCESS;
}

static const VkExtensionProperties instanceExtensionProperties[] = {
	{ VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME, VK_KHR_DEVICE_GROUP_CREATION_SPEC_VERSION },
	{ VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME, VK_KHR_EXTERNAL_FENCE_CAPABILITIES_SPEC_VERSION },
	{ VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME, VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_SPEC_VERSION },
	{ VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME, VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_SPEC_VERSION },
	{ VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_SPEC_VERSION },
#ifndef __ANDROID__
	{ VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_SURFACE_SPEC_VERSION },
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
	{ VK_KHR_XCB_SURFACE_EXTENSION_NAME, VK_KHR_XCB_SURFACE_SPEC_VERSION },
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
	{ VK_KHR_XLIB_SURFACE_EXTENSION_NAME, VK_KHR_XLIB_SURFACE_SPEC_VERSION },
#endif
#ifdef VK_USE_PLATFORM_MACOS_MVK
	{ VK_MVK_MACOS_SURFACE_EXTENSION_NAME, VK_MVK_MACOS_SURFACE_SPEC_VERSION },
#endif
#ifdef VK_USE_PLATFORM_METAL_EXT
	{ VK_EXT_METAL_SURFACE_EXTENSION_NAME, VK_EXT_METAL_SURFACE_SPEC_VERSION },
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
	{ VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_SPEC_VERSION },
#endif
};

static const VkExtensionProperties deviceExtensionProperties[] = {
	{ VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME, VK_KHR_DRIVER_PROPERTIES_SPEC_VERSION },
	// Vulkan 1.1 promoted extensions
	{ VK_KHR_16BIT_STORAGE_EXTENSION_NAME, VK_KHR_16BIT_STORAGE_SPEC_VERSION },
	{ VK_KHR_BIND_MEMORY_2_EXTENSION_NAME, VK_KHR_BIND_MEMORY_2_SPEC_VERSION },
	{ VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME, VK_KHR_CREATE_RENDERPASS_2_SPEC_VERSION },
	{ VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME, VK_KHR_DEDICATED_ALLOCATION_SPEC_VERSION },
	{ VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME, VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_SPEC_VERSION },
	{ VK_KHR_DEVICE_GROUP_EXTENSION_NAME, VK_KHR_DEVICE_GROUP_SPEC_VERSION },
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
	{ VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME, VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_SPEC_VERSION },
	// Only 1.1 core version of this is supported. The extension has additional requirements
	//{ VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME, VK_KHR_SHADER_DRAW_PARAMETERS_SPEC_VERSION },
	{ VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME, VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_SPEC_VERSION },
	// Only 1.1 core version of this is supported. The extension has additional requirements
	//{ VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME, VK_KHR_VARIABLE_POINTERS_SPEC_VERSION },
	{ VK_EXT_QUEUE_FAMILY_FOREIGN_EXTENSION_NAME, VK_EXT_QUEUE_FAMILY_FOREIGN_SPEC_VERSION },
	// The following extension is only used to add support for Bresenham lines
	{ VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME, VK_EXT_LINE_RASTERIZATION_SPEC_VERSION },
	// The following extension is used by ANGLE to emulate blitting the stencil buffer
	{ VK_EXT_SHADER_STENCIL_EXPORT_EXTENSION_NAME, VK_EXT_SHADER_STENCIL_EXPORT_SPEC_VERSION },
#ifndef __ANDROID__
	// We fully support the KHR_swapchain v70 additions, so just track the spec version.
	{ VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SWAPCHAIN_SPEC_VERSION },
#else
	// We only support V7 of this extension. Missing functionality: in V8,
	// it becomes possible to pass a VkNativeBufferANDROID structure to
	// vkBindImageMemory2. Android's swapchain implementation does this in
	// order to support passing VkBindImageMemorySwapchainInfoKHR
	// (from KHR_swapchain v70) to vkBindImageMemory2.
	{ VK_ANDROID_NATIVE_BUFFER_EXTENSION_NAME, 7 },
#endif
#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
	{ VK_ANDROID_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER_EXTENSION_NAME, VK_ANDROID_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER_SPEC_VERSION },
#endif
#if SWIFTSHADER_EXTERNAL_SEMAPHORE_OPAQUE_FD
	{ VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME, VK_KHR_EXTERNAL_SEMAPHORE_FD_SPEC_VERSION },
#endif
#if SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD
	{ VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME, VK_KHR_EXTERNAL_MEMORY_FD_SPEC_VERSION },
#endif

	{ VK_EXT_EXTERNAL_MEMORY_HOST_EXTENSION_NAME, VK_EXT_EXTERNAL_MEMORY_HOST_SPEC_VERSION },

#if VK_USE_PLATFORM_FUCHSIA
	{ VK_FUCHSIA_EXTERNAL_SEMAPHORE_EXTENSION_NAME, VK_FUCHSIA_EXTERNAL_SEMAPHORE_SPEC_VERSION },
#endif
	{ VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME, VK_EXT_PROVOKING_VERTEX_SPEC_VERSION },
};

VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkInstance *pInstance)
{
	TRACE("(const VkInstanceCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkInstance* pInstance = %p)",
	      pCreateInfo, pAllocator, pInstance);

	initializeLibrary();

	if(pCreateInfo->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	if(pCreateInfo->enabledLayerCount != 0)
	{
		UNIMPLEMENTED("b/148240133: pCreateInfo->enabledLayerCount != 0");  // FIXME(b/148240133)
	}

	uint32_t extensionPropertiesCount = sizeof(instanceExtensionProperties) / sizeof(instanceExtensionProperties[0]);
	for(uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; ++i)
	{
		if(!HasExtensionProperty(pCreateInfo->ppEnabledExtensionNames[i], instanceExtensionProperties, extensionPropertiesCount))
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	if(pCreateInfo->pNext)
	{
		const VkBaseInStructure *createInfo = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext);
		switch(createInfo->sType)
		{
			case VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO:
				// According to the Vulkan spec, section 2.7.2. Implicit Valid Usage:
				// "The values VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO and
				//  VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO are reserved for
				//  internal use by the loader, and do not have corresponding
				//  Vulkan structures in this Specification."
				break;
			default:
				LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(createInfo->sType).c_str());
				break;
		}
	}

	*pInstance = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	VkResult result = vk::DispatchablePhysicalDevice::Create(pAllocator, pCreateInfo, &physicalDevice);
	if(result != VK_SUCCESS)
	{
		return result;
	}

	result = vk::DispatchableInstance::Create(pAllocator, pCreateInfo, pInstance, physicalDevice);
	if(result != VK_SUCCESS)
	{
		vk::destroy(physicalDevice, pAllocator);
		return result;
	}

	return result;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkInstance instance = %p, const VkAllocationCallbacks* pAllocator = %p)", instance, pAllocator);

	vk::destroy(instance, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDevices(VkInstance instance, uint32_t *pPhysicalDeviceCount, VkPhysicalDevice *pPhysicalDevices)
{
	TRACE("(VkInstance instance = %p, uint32_t* pPhysicalDeviceCount = %p, VkPhysicalDevice* pPhysicalDevices = %p)",
	      instance, pPhysicalDeviceCount, pPhysicalDevices);

	return vk::Cast(instance)->getPhysicalDevices(pPhysicalDeviceCount, pPhysicalDevices);
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures *pFeatures)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, VkPhysicalDeviceFeatures* pFeatures = %p)",
	      physicalDevice, pFeatures);

	*pFeatures = vk::Cast(physicalDevice)->getFeatures();
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties *pFormatProperties)
{
	TRACE("GetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice = %p, VkFormat format = %d, VkFormatProperties* pFormatProperties = %p)",
	      physicalDevice, (int)format, pFormatProperties);

	vk::Cast(physicalDevice)->getFormatProperties(format, pFormatProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties *pImageFormatProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, VkFormat format = %d, VkImageType type = %d, VkImageTiling tiling = %d, VkImageUsageFlags usage = %d, VkImageCreateFlags flags = %d, VkImageFormatProperties* pImageFormatProperties = %p)",
	      physicalDevice, (int)format, (int)type, (int)tiling, usage, flags, pImageFormatProperties);

	// "If the combination of parameters to vkGetPhysicalDeviceImageFormatProperties is not supported by the implementation
	//  for use in vkCreateImage, then all members of VkImageFormatProperties will be filled with zero."
	memset(pImageFormatProperties, 0, sizeof(VkImageFormatProperties));

	VkFormatProperties properties;
	vk::Cast(physicalDevice)->getFormatProperties(format, &properties);

	VkFormatFeatureFlags features;
	switch(tiling)
	{
		case VK_IMAGE_TILING_LINEAR:
			features = properties.linearTilingFeatures;
			break;

		case VK_IMAGE_TILING_OPTIMAL:
			features = properties.optimalTilingFeatures;
			break;

		default:
			UNSUPPORTED("VkImageTiling %d", int(tiling));
			features = 0;
	}

	if(features == 0)
	{
		return VK_ERROR_FORMAT_NOT_SUPPORTED;
	}

	// Check for usage conflict with features
	if((usage & VK_IMAGE_USAGE_SAMPLED_BIT) && !(features & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
	{
		return VK_ERROR_FORMAT_NOT_SUPPORTED;
	}

	if((usage & VK_IMAGE_USAGE_STORAGE_BIT) && !(features & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT))
	{
		return VK_ERROR_FORMAT_NOT_SUPPORTED;
	}

	if((usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) && !(features & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT))
	{
		return VK_ERROR_FORMAT_NOT_SUPPORTED;
	}

	if((usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) && !(features & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
	{
		return VK_ERROR_FORMAT_NOT_SUPPORTED;
	}

	if((usage & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) && !(features & (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)))
	{
		return VK_ERROR_FORMAT_NOT_SUPPORTED;
	}

	if((usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) && !(features & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT))
	{
		return VK_ERROR_FORMAT_NOT_SUPPORTED;
	}

	if((usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT) && !(features & VK_FORMAT_FEATURE_TRANSFER_DST_BIT))
	{
		return VK_ERROR_FORMAT_NOT_SUPPORTED;
	}

	auto allRecognizedUsageBits = VK_IMAGE_USAGE_SAMPLED_BIT |
	                              VK_IMAGE_USAGE_STORAGE_BIT |
	                              VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
	                              VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
	                              VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
	                              VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
	                              VK_IMAGE_USAGE_TRANSFER_DST_BIT |
	                              VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
	ASSERT(!(usage & ~(allRecognizedUsageBits)));

	// "Images created with tiling equal to VK_IMAGE_TILING_LINEAR have further restrictions on their limits and capabilities
	//  compared to images created with tiling equal to VK_IMAGE_TILING_OPTIMAL."
	if(tiling == VK_IMAGE_TILING_LINEAR)
	{
		if(type != VK_IMAGE_TYPE_2D)
		{
			return VK_ERROR_FORMAT_NOT_SUPPORTED;
		}

		if(vk::Format(format).isDepth() || vk::Format(format).isStencil())
		{
			return VK_ERROR_FORMAT_NOT_SUPPORTED;
		}
	}

	// "Images created with a format from one of those listed in Formats requiring sampler Y'CBCR conversion for VK_IMAGE_ASPECT_COLOR_BIT image views
	//  have further restrictions on their limits and capabilities compared to images created with other formats."
	if(vk::Format(format).isYcbcrFormat())
	{
		if(type != VK_IMAGE_TYPE_2D)
		{
			return VK_ERROR_FORMAT_NOT_SUPPORTED;
		}
	}

	vk::Cast(physicalDevice)->getImageFormatProperties(format, type, tiling, usage, flags, pImageFormatProperties);

	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties *pProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, VkPhysicalDeviceProperties* pProperties = %p)",
	      physicalDevice, pProperties);

	*pProperties = vk::Cast(physicalDevice)->getProperties();
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t *pQueueFamilyPropertyCount, VkQueueFamilyProperties *pQueueFamilyProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, uint32_t* pQueueFamilyPropertyCount = %p, VkQueueFamilyProperties* pQueueFamilyProperties = %p))", physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);

	if(!pQueueFamilyProperties)
	{
		*pQueueFamilyPropertyCount = vk::Cast(physicalDevice)->getQueueFamilyPropertyCount();
	}
	else
	{
		vk::Cast(physicalDevice)->getQueueFamilyProperties(*pQueueFamilyPropertyCount, pQueueFamilyProperties);
	}
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties *pMemoryProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, VkPhysicalDeviceMemoryProperties* pMemoryProperties = %p)", physicalDevice, pMemoryProperties);

	*pMemoryProperties = vk::Cast(physicalDevice)->getMemoryProperties();
}

VK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char *pName)
{
	TRACE("(VkInstance instance = %p, const char* pName = %p)", instance, pName);

	return vk::GetInstanceProcAddr(vk::Cast(instance), pName);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice device, const char *pName)
{
	TRACE("(VkDevice device = %p, const char* pName = %p)", device, pName);

	return vk::GetDeviceProcAddr(vk::Cast(device), pName);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDevice *pDevice)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, const VkDeviceCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkDevice* pDevice = %p)",
	      physicalDevice, pCreateInfo, pAllocator, pDevice);

	if(pCreateInfo->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	if(pCreateInfo->enabledLayerCount != 0)
	{
		// "The ppEnabledLayerNames and enabledLayerCount members of VkDeviceCreateInfo are deprecated and their values must be ignored by implementations."
		UNSUPPORTED("pCreateInfo->enabledLayerCount != 0");
	}

	uint32_t extensionPropertiesCount = sizeof(deviceExtensionProperties) / sizeof(deviceExtensionProperties[0]);
	for(uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; ++i)
	{
		if(!HasExtensionProperty(pCreateInfo->ppEnabledExtensionNames[i], deviceExtensionProperties, extensionPropertiesCount))
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	const VkBaseInStructure *extensionCreateInfo = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext);

	const VkPhysicalDeviceFeatures *enabledFeatures = pCreateInfo->pEnabledFeatures;

	while(extensionCreateInfo)
	{
		// Casting to a long since some structures, such as
		// VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT
		// are not enumerated in the official Vulkan header
		switch((long)(extensionCreateInfo->sType))
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
				ASSERT(!pCreateInfo->pEnabledFeatures);  // "If the pNext chain includes a VkPhysicalDeviceFeatures2 structure, then pEnabledFeatures must be NULL"

				const VkPhysicalDeviceFeatures2 *physicalDeviceFeatures2 = reinterpret_cast<const VkPhysicalDeviceFeatures2 *>(extensionCreateInfo);

				enabledFeatures = &physicalDeviceFeatures2->features;
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES:
			{
				const VkPhysicalDeviceSamplerYcbcrConversionFeatures *samplerYcbcrConversionFeatures = reinterpret_cast<const VkPhysicalDeviceSamplerYcbcrConversionFeatures *>(extensionCreateInfo);

				// YCbCr conversion is supported.
				// samplerYcbcrConversionFeatures->samplerYcbcrConversion can be VK_TRUE or VK_FALSE.
				// No action needs to be taken on our end in either case; it's the apps responsibility that
				// "To create a sampler Y'CbCr conversion, the samplerYcbcrConversion feature must be enabled."
				(void)samplerYcbcrConversionFeatures->samplerYcbcrConversion;
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES:
			{
				const VkPhysicalDevice16BitStorageFeatures *storage16BitFeatures = reinterpret_cast<const VkPhysicalDevice16BitStorageFeatures *>(extensionCreateInfo);

				if(storage16BitFeatures->storageBuffer16BitAccess != VK_FALSE ||
				   storage16BitFeatures->uniformAndStorageBuffer16BitAccess != VK_FALSE ||
				   storage16BitFeatures->storagePushConstant16 != VK_FALSE ||
				   storage16BitFeatures->storageInputOutput16 != VK_FALSE)
				{
					return VK_ERROR_FEATURE_NOT_PRESENT;
				}
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES:
			{
				const VkPhysicalDeviceVariablePointerFeatures *variablePointerFeatures = reinterpret_cast<const VkPhysicalDeviceVariablePointerFeatures *>(extensionCreateInfo);

				if(variablePointerFeatures->variablePointersStorageBuffer != VK_FALSE ||
				   variablePointerFeatures->variablePointers != VK_FALSE)
				{
					return VK_ERROR_FEATURE_NOT_PRESENT;
				}
			}
			break;
			case VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO:
			{
				const VkDeviceGroupDeviceCreateInfo *groupDeviceCreateInfo = reinterpret_cast<const VkDeviceGroupDeviceCreateInfo *>(extensionCreateInfo);

				if((groupDeviceCreateInfo->physicalDeviceCount != 1) ||
				   (groupDeviceCreateInfo->pPhysicalDevices[0] != physicalDevice))
				{
					return VK_ERROR_FEATURE_NOT_PRESENT;
				}
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES:
			{
				const VkPhysicalDeviceMultiviewFeatures *multiviewFeatures = reinterpret_cast<const VkPhysicalDeviceMultiviewFeatures *>(extensionCreateInfo);

				if(multiviewFeatures->multiviewGeometryShader ||
				   multiviewFeatures->multiviewTessellationShader)
				{
					return VK_ERROR_FEATURE_NOT_PRESENT;
				}
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES:
			{
				const VkPhysicalDeviceShaderDrawParametersFeatures *shaderDrawParametersFeatures = reinterpret_cast<const VkPhysicalDeviceShaderDrawParametersFeatures *>(extensionCreateInfo);

				if(shaderDrawParametersFeatures->shaderDrawParameters)
				{
					return VK_ERROR_FEATURE_NOT_PRESENT;
				}
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES_KHR:
			{
				const VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *shaderDrawParametersFeatures = reinterpret_cast<const VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *>(extensionCreateInfo);

				// Separate depth and stencil layouts is already supported
				(void)(shaderDrawParametersFeatures->separateDepthStencilLayouts);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT:
			{
				const VkPhysicalDeviceLineRasterizationFeaturesEXT *lineRasterizationFeatures = reinterpret_cast<const VkPhysicalDeviceLineRasterizationFeaturesEXT *>(extensionCreateInfo);
				if((lineRasterizationFeatures->smoothLines != VK_FALSE) ||
				   (lineRasterizationFeatures->stippledBresenhamLines != VK_FALSE) ||
				   (lineRasterizationFeatures->stippledRectangularLines != VK_FALSE) ||
				   (lineRasterizationFeatures->stippledSmoothLines != VK_FALSE))
				{
					return VK_ERROR_FEATURE_NOT_PRESENT;
				}
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT:
			{
				const VkPhysicalDeviceProvokingVertexFeaturesEXT *provokingVertexFeatures = reinterpret_cast<const VkPhysicalDeviceProvokingVertexFeaturesEXT *>(extensionCreateInfo);

				// Provoking vertex is supported.
				// provokingVertexFeatures->provokingVertexLast can be VK_TRUE or VK_FALSE.
				// No action needs to be taken on our end in either case; it's the apps responsibility to check
				// that the provokingVertexLast feature is enabled before using the provoking vertex convention.
				(void)provokingVertexFeatures->provokingVertexLast;
			}
			break;
			default:
				// "the [driver] must skip over, without processing (other than reading the sType and pNext members) any structures in the chain with sType values not defined by [supported extenions]"
				LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(extensionCreateInfo->sType).c_str());
				break;
		}

		extensionCreateInfo = extensionCreateInfo->pNext;
	}

	ASSERT(pCreateInfo->queueCreateInfoCount > 0);

	if(enabledFeatures)
	{
		if(!vk::Cast(physicalDevice)->hasFeatures(*enabledFeatures))
		{
			return VK_ERROR_FEATURE_NOT_PRESENT;
		}
	}

	uint32_t queueFamilyPropertyCount = vk::Cast(physicalDevice)->getQueueFamilyPropertyCount();

	for(uint32_t i = 0; i < pCreateInfo->queueCreateInfoCount; i++)
	{
		const VkDeviceQueueCreateInfo &queueCreateInfo = pCreateInfo->pQueueCreateInfos[i];
		if(queueCreateInfo.flags != 0)
		{
			UNSUPPORTED("pCreateInfo->pQueueCreateInfos[%d]->flags %d", i, queueCreateInfo.flags);
		}

		auto extInfo = reinterpret_cast<VkBaseInStructure const *>(queueCreateInfo.pNext);
		while(extInfo)
		{
			LOG_TRAP("pCreateInfo->pQueueCreateInfos[%d].pNext sType = %s", i, vk::Stringify(extInfo->sType).c_str());
			extInfo = extInfo->pNext;
		}

		ASSERT(queueCreateInfo.queueFamilyIndex < queueFamilyPropertyCount);
		(void)queueFamilyPropertyCount;  // Silence unused variable warning
	}

	auto scheduler = getOrCreateScheduler();
	return vk::DispatchableDevice::Create(pAllocator, pCreateInfo, pDevice, vk::Cast(physicalDevice), enabledFeatures, scheduler);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDevice(VkDevice device, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, const VkAllocationCallbacks* pAllocator = %p)", device, pAllocator);

	vk::destroy(device, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pPropertyCount, VkExtensionProperties *pProperties)
{
	TRACE("(const char* pLayerName = %p, uint32_t* pPropertyCount = %p, VkExtensionProperties* pProperties = %p)",
	      pLayerName, pPropertyCount, pProperties);

	uint32_t extensionPropertiesCount = sizeof(instanceExtensionProperties) / sizeof(instanceExtensionProperties[0]);

	if(!pProperties)
	{
		*pPropertyCount = extensionPropertiesCount;
		return VK_SUCCESS;
	}

	auto toCopy = std::min(*pPropertyCount, extensionPropertiesCount);
	for(uint32_t i = 0; i < toCopy; i++)
	{
		pProperties[i] = instanceExtensionProperties[i];
	}

	*pPropertyCount = toCopy;
	return (toCopy < extensionPropertiesCount) ? VK_INCOMPLETE : VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char *pLayerName, uint32_t *pPropertyCount, VkExtensionProperties *pProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, const char* pLayerName, uint32_t* pPropertyCount = %p, VkExtensionProperties* pProperties = %p)", physicalDevice, pPropertyCount, pProperties);

	uint32_t extensionPropertiesCount = sizeof(deviceExtensionProperties) / sizeof(deviceExtensionProperties[0]);

	if(!pProperties)
	{
		*pPropertyCount = extensionPropertiesCount;
		return VK_SUCCESS;
	}

	auto toCopy = std::min(*pPropertyCount, extensionPropertiesCount);
	for(uint32_t i = 0; i < toCopy; i++)
	{
		pProperties[i] = deviceExtensionProperties[i];
	}

	*pPropertyCount = toCopy;
	return (toCopy < extensionPropertiesCount) ? VK_INCOMPLETE : VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(uint32_t *pPropertyCount, VkLayerProperties *pProperties)
{
	TRACE("(uint32_t* pPropertyCount = %p, VkLayerProperties* pProperties = %p)", pPropertyCount, pProperties);

	if(!pProperties)
	{
		*pPropertyCount = 0;
		return VK_SUCCESS;
	}

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t *pPropertyCount, VkLayerProperties *pProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, uint32_t* pPropertyCount = %p, VkLayerProperties* pProperties = %p)", physicalDevice, pPropertyCount, pProperties);

	if(!pProperties)
	{
		*pPropertyCount = 0;
		return VK_SUCCESS;
	}

	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue *pQueue)
{
	TRACE("(VkDevice device = %p, uint32_t queueFamilyIndex = %d, uint32_t queueIndex = %d, VkQueue* pQueue = %p)",
	      device, queueFamilyIndex, queueIndex, pQueue);

	*pQueue = vk::Cast(device)->getQueue(queueFamilyIndex, queueIndex);
}

VKAPI_ATTR VkResult VKAPI_CALL vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo *pSubmits, VkFence fence)
{
	TRACE("(VkQueue queue = %p, uint32_t submitCount = %d, const VkSubmitInfo* pSubmits = %p, VkFence fence = %p)",
	      queue, submitCount, pSubmits, static_cast<void *>(fence));

	return vk::Cast(queue)->submit(submitCount, pSubmits, vk::Cast(fence));
}

VKAPI_ATTR VkResult VKAPI_CALL vkQueueWaitIdle(VkQueue queue)
{
	TRACE("(VkQueue queue = %p)", queue);

	return vk::Cast(queue)->waitIdle();
}

VKAPI_ATTR VkResult VKAPI_CALL vkDeviceWaitIdle(VkDevice device)
{
	TRACE("(VkDevice device = %p)", device);

	return vk::Cast(device)->waitIdle();
}

VKAPI_ATTR VkResult VKAPI_CALL vkAllocateMemory(VkDevice device, const VkMemoryAllocateInfo *pAllocateInfo, const VkAllocationCallbacks *pAllocator, VkDeviceMemory *pMemory)
{
	TRACE("(VkDevice device = %p, const VkMemoryAllocateInfo* pAllocateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkDeviceMemory* pMemory = %p)",
	      device, pAllocateInfo, pAllocator, pMemory);

	const VkBaseInStructure *allocationInfo = reinterpret_cast<const VkBaseInStructure *>(pAllocateInfo->pNext);
	while(allocationInfo)
	{
		switch(allocationInfo->sType)
		{
			case VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO:
				// This can safely be ignored, as the Vulkan spec mentions:
				// "If the pNext chain includes a VkMemoryDedicatedAllocateInfo structure, then that structure
				//  includes a handle of the sole buffer or image resource that the memory *can* be bound to."
				break;
			case VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO:
				// This extension controls on which physical devices the memory gets allocated.
				// SwiftShader only has a single physical device, so this extension does nothing in this case.
				break;
#if SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD
			case VK_STRUCTURE_TYPE_IMPORT_MEMORY_FD_INFO_KHR:
			{
				auto *importInfo = reinterpret_cast<const VkImportMemoryFdInfoKHR *>(allocationInfo);
				if(importInfo->handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT)
				{
					UNSUPPORTED("importInfo->handleType %u", importInfo->handleType);
					return VK_ERROR_INVALID_EXTERNAL_HANDLE;
				}
				break;
			}
#endif  // SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD
			case VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO:
			{
				auto *exportInfo = reinterpret_cast<const VkExportMemoryAllocateInfo *>(allocationInfo);
				switch(exportInfo->handleTypes)
				{
#if SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD
					case VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT:
						break;
#endif
#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
					case VK_EXTERNAL_MEMORY_HANDLE_TYPE_ANDROID_HARDWARE_BUFFER_BIT_ANDROID:
						break;
#endif
					default:
						UNSUPPORTED("exportInfo->handleTypes %u", exportInfo->handleTypes);
						return VK_ERROR_INVALID_EXTERNAL_HANDLE;
				}
				break;
			}
#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
			case VK_STRUCTURE_TYPE_IMPORT_ANDROID_HARDWARE_BUFFER_INFO_ANDROID:
				// Ignore
				break;
#endif  // SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
			case VK_STRUCTURE_TYPE_IMPORT_MEMORY_HOST_POINTER_INFO_EXT:
			{
				auto *importInfo = reinterpret_cast<const VkImportMemoryHostPointerInfoEXT *>(allocationInfo);
				if(importInfo->handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_ALLOCATION_BIT_EXT && importInfo->handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_MAPPED_FOREIGN_MEMORY_BIT_EXT)
				{
					UNSUPPORTED("importInfo->handleType %u", importInfo->handleType);
					return VK_ERROR_INVALID_EXTERNAL_HANDLE;
				}
				break;
			}
			default:
				LOG_TRAP("pAllocateInfo->pNext sType = %s", vk::Stringify(allocationInfo->sType).c_str());
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

VKAPI_ATTR void VKAPI_CALL vkFreeMemory(VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkDeviceMemory memory = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(memory), pAllocator);

	vk::destroy(memory, pAllocator);
}

#if SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD
VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryFdKHR(VkDevice device, const VkMemoryGetFdInfoKHR *getFdInfo, int *pFd)
{
	TRACE("(VkDevice device = %p, const VkMemoryGetFdInfoKHR* getFdInfo = %p, int* pFd = %p",
	      device, getFdInfo, pFd);

	if(getFdInfo->handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT)
	{
		UNSUPPORTED("pGetFdInfo->handleType %u", getFdInfo->handleType);
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}
	return vk::Cast(getFdInfo->memory)->exportFd(pFd);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryFdPropertiesKHR(VkDevice device, VkExternalMemoryHandleTypeFlagBits handleType, int fd, VkMemoryFdPropertiesKHR *pMemoryFdProperties)
{
	TRACE("(VkDevice device = %p, VkExternalMemoryHandleTypeFlagBits handleType = %x, int fd = %d, VkMemoryFdPropertiesKHR* pMemoryFdProperties = %p)",
	      device, handleType, fd, pMemoryFdProperties);

	if(handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT)
	{
		UNSUPPORTED("handleType %u", handleType);
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}

	if(fd < 0)
	{
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}

	const VkPhysicalDeviceMemoryProperties &memoryProperties =
	    vk::Cast(device)->getPhysicalDevice()->getMemoryProperties();

	// All SwiftShader memory types support this!
	pMemoryFdProperties->memoryTypeBits = (1U << memoryProperties.memoryTypeCount) - 1U;

	return VK_SUCCESS;
}
#endif  // SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD

VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryHostPointerPropertiesEXT(VkDevice device, VkExternalMemoryHandleTypeFlagBits handleType, const void *pHostPointer, VkMemoryHostPointerPropertiesEXT *pMemoryHostPointerProperties)
{
	TRACE("(VkDevice device = %p, VkExternalMemoryHandleTypeFlagBits handleType = %x, const void *pHostPointer = %p, VkMemoryHostPointerPropertiesEXT *pMemoryHostPointerProperties = %p)",
	      device, handleType, pHostPointer, pMemoryHostPointerProperties);

	if(handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_ALLOCATION_BIT_EXT && handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_MAPPED_FOREIGN_MEMORY_BIT_EXT)
	{
		UNSUPPORTED("handleType %u", handleType);
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}
	pMemoryHostPointerProperties->memoryTypeBits = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

	return VK_SUCCESS;
}

#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryAndroidHardwareBufferANDROID(VkDevice device, const VkMemoryGetAndroidHardwareBufferInfoANDROID *pInfo, struct AHardwareBuffer **pBuffer)
{
	TRACE("(VkDevice device = %p, const VkMemoryGetAndroidHardwareBufferInfoANDROID *pInfo = %p, struct AHardwareBuffer **pBuffer = %p)",
	      device, pInfo, pBuffer);

	return vk::Cast(pInfo->memory)->exportAhb(pBuffer);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetAndroidHardwareBufferPropertiesANDROID(VkDevice device, const struct AHardwareBuffer *buffer, VkAndroidHardwareBufferPropertiesANDROID *pProperties)
{
	TRACE("(VkDevice device = %p, const struct AHardwareBuffer *buffer = %p, VkAndroidHardwareBufferPropertiesANDROID *pProperties = %p)",
	      device, buffer, pProperties);

	return vk::DeviceMemory::getAhbProperties(buffer, pProperties);
}
#endif  // SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER

VKAPI_ATTR VkResult VKAPI_CALL vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void **ppData)
{
	TRACE("(VkDevice device = %p, VkDeviceMemory memory = %p, VkDeviceSize offset = %d, VkDeviceSize size = %d, VkMemoryMapFlags flags = %d, void** ppData = %p)",
	      device, static_cast<void *>(memory), int(offset), int(size), flags, ppData);

	if(flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("flags %d", int(flags));
	}

	return vk::Cast(memory)->map(offset, size, ppData);
}

VKAPI_ATTR void VKAPI_CALL vkUnmapMemory(VkDevice device, VkDeviceMemory memory)
{
	TRACE("(VkDevice device = %p, VkDeviceMemory memory = %p)", device, static_cast<void *>(memory));

	// Noop, memory will be released when the DeviceMemory object is released
}

VKAPI_ATTR VkResult VKAPI_CALL vkFlushMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange *pMemoryRanges)
{
	TRACE("(VkDevice device = %p, uint32_t memoryRangeCount = %d, const VkMappedMemoryRange* pMemoryRanges = %p)",
	      device, memoryRangeCount, pMemoryRanges);

	// Noop, host and device memory are the same to SwiftShader

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkInvalidateMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange *pMemoryRanges)
{
	TRACE("(VkDevice device = %p, uint32_t memoryRangeCount = %d, const VkMappedMemoryRange* pMemoryRanges = %p)",
	      device, memoryRangeCount, pMemoryRanges);

	// Noop, host and device memory are the same to SwiftShader

	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetDeviceMemoryCommitment(VkDevice pDevice, VkDeviceMemory pMemory, VkDeviceSize *pCommittedMemoryInBytes)
{
	TRACE("(VkDevice device = %p, VkDeviceMemory memory = %p, VkDeviceSize* pCommittedMemoryInBytes = %p)",
	      pDevice, static_cast<void *>(pMemory), pCommittedMemoryInBytes);

	auto memory = vk::Cast(pMemory);

#if !defined(NDEBUG) || defined(DCHECK_ALWAYS_ON)
	const auto &memoryProperties = vk::Cast(pDevice)->getPhysicalDevice()->getMemoryProperties();
	uint32_t typeIndex = memory->getMemoryTypeIndex();
	ASSERT(typeIndex < memoryProperties.memoryTypeCount);
	ASSERT(memoryProperties.memoryTypes[typeIndex].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT);
#endif

	*pCommittedMemoryInBytes = memory->getCommittedMemoryInBytes();
}

VKAPI_ATTR VkResult VKAPI_CALL vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset)
{
	TRACE("(VkDevice device = %p, VkBuffer buffer = %p, VkDeviceMemory memory = %p, VkDeviceSize memoryOffset = %d)",
	      device, static_cast<void *>(buffer), static_cast<void *>(memory), int(memoryOffset));

	if(!vk::Cast(buffer)->canBindToMemory(vk::Cast(memory)))
	{
		UNSUPPORTED("vkBindBufferMemory with invalid external memory");
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}
	vk::Cast(buffer)->bind(vk::Cast(memory), memoryOffset);
	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkBindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset)
{
	TRACE("(VkDevice device = %p, VkImage image = %p, VkDeviceMemory memory = %p, VkDeviceSize memoryOffset = %d)",
	      device, static_cast<void *>(image), static_cast<void *>(memory), int(memoryOffset));

	if(!vk::Cast(image)->canBindToMemory(vk::Cast(memory)))
	{
		UNSUPPORTED("vkBindImageMemory with invalid external memory");
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}
	vk::Cast(image)->bind(vk::Cast(memory), memoryOffset);
	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer, VkMemoryRequirements *pMemoryRequirements)
{
	TRACE("(VkDevice device = %p, VkBuffer buffer = %p, VkMemoryRequirements* pMemoryRequirements = %p)",
	      device, static_cast<void *>(buffer), pMemoryRequirements);

	*pMemoryRequirements = vk::Cast(buffer)->getMemoryRequirements();
}

VKAPI_ATTR void VKAPI_CALL vkGetImageMemoryRequirements(VkDevice device, VkImage image, VkMemoryRequirements *pMemoryRequirements)
{
	TRACE("(VkDevice device = %p, VkImage image = %p, VkMemoryRequirements* pMemoryRequirements = %p)",
	      device, static_cast<void *>(image), pMemoryRequirements);

	*pMemoryRequirements = vk::Cast(image)->getMemoryRequirements();
}

VKAPI_ATTR void VKAPI_CALL vkGetImageSparseMemoryRequirements(VkDevice device, VkImage image, uint32_t *pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements *pSparseMemoryRequirements)
{
	TRACE("(VkDevice device = %p, VkImage image = %p, uint32_t* pSparseMemoryRequirementCount = %p, VkSparseImageMemoryRequirements* pSparseMemoryRequirements = %p)",
	      device, static_cast<void *>(image), pSparseMemoryRequirementCount, pSparseMemoryRequirements);

	// The 'sparseBinding' feature is not supported, so images can not be created with the VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT flag.
	// "If the image was not created with VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT then pSparseMemoryRequirementCount will be set to zero and pSparseMemoryRequirements will not be written to."
	*pSparseMemoryRequirementCount = 0;
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceSparseImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling, uint32_t *pPropertyCount, VkSparseImageFormatProperties *pProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, VkFormat format = %d, VkImageType type = %d, VkSampleCountFlagBits samples = %d, VkImageUsageFlags usage = %d, VkImageTiling tiling = %d, uint32_t* pPropertyCount = %p, VkSparseImageFormatProperties* pProperties = %p)",
	      physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);

	// We do not support sparse images.
	*pPropertyCount = 0;
}

VKAPI_ATTR VkResult VKAPI_CALL vkQueueBindSparse(VkQueue queue, uint32_t bindInfoCount, const VkBindSparseInfo *pBindInfo, VkFence fence)
{
	TRACE("()");
	UNSUPPORTED("vkQueueBindSparse");
	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateFence(VkDevice device, const VkFenceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkFence *pFence)
{
	TRACE("(VkDevice device = %p, const VkFenceCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkFence* pFence = %p)",
	      device, pCreateInfo, pAllocator, pFence);

	auto *nextInfo = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext);
	while(nextInfo)
	{
		LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(nextInfo->sType).c_str());
		nextInfo = nextInfo->pNext;
	}

	return vk::Fence::Create(pAllocator, pCreateInfo, pFence);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyFence(VkDevice device, VkFence fence, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkFence fence = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(fence), pAllocator);

	vk::destroy(fence, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetFences(VkDevice device, uint32_t fenceCount, const VkFence *pFences)
{
	TRACE("(VkDevice device = %p, uint32_t fenceCount = %d, const VkFence* pFences = %p)",
	      device, fenceCount, pFences);

	for(uint32_t i = 0; i < fenceCount; i++)
	{
		vk::Cast(pFences[i])->reset();
	}

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetFenceStatus(VkDevice device, VkFence fence)
{
	TRACE("(VkDevice device = %p, VkFence fence = %p)", device, static_cast<void *>(fence));

	return vk::Cast(fence)->getStatus();
}

VKAPI_ATTR VkResult VKAPI_CALL vkWaitForFences(VkDevice device, uint32_t fenceCount, const VkFence *pFences, VkBool32 waitAll, uint64_t timeout)
{
	TRACE("(VkDevice device = %p, uint32_t fenceCount = %d, const VkFence* pFences = %p, VkBool32 waitAll = %d, uint64_t timeout = %d)",
	      device, int(fenceCount), pFences, int(waitAll), int(timeout));

	return vk::Cast(device)->waitForFences(fenceCount, pFences, waitAll, timeout);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSemaphore *pSemaphore)
{
	TRACE("(VkDevice device = %p, const VkSemaphoreCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkSemaphore* pSemaphore = %p)",
	      device, pCreateInfo, pAllocator, pSemaphore);

	if(pCreateInfo->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	return vk::Semaphore::Create(pAllocator, pCreateInfo, pSemaphore, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL vkDestroySemaphore(VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkSemaphore semaphore = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(semaphore), pAllocator);

	vk::destroy(semaphore, pAllocator);
}

#if SWIFTSHADER_EXTERNAL_SEMAPHORE_OPAQUE_FD
VKAPI_ATTR VkResult VKAPI_CALL vkGetSemaphoreFdKHR(VkDevice device, const VkSemaphoreGetFdInfoKHR *pGetFdInfo, int *pFd)
{
	TRACE("(VkDevice device = %p, const VkSemaphoreGetFdInfoKHR* pGetFdInfo = %p, int* pFd = %p)",
	      device, static_cast<const void *>(pGetFdInfo), static_cast<void *>(pFd));

	if(pGetFdInfo->handleType != VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT)
	{
		UNSUPPORTED("pGetFdInfo->handleType %d", int(pGetFdInfo->handleType));
	}

	return vk::Cast(pGetFdInfo->semaphore)->exportFd(pFd);
}

VKAPI_ATTR VkResult VKAPI_CALL vkImportSemaphoreFdKHR(VkDevice device, const VkImportSemaphoreFdInfoKHR *pImportSemaphoreInfo)
{
	TRACE("(VkDevice device = %p, const VkImportSemaphoreFdInfoKHR* pImportSemaphoreInfo = %p",
	      device, static_cast<const void *>(pImportSemaphoreInfo));

	if(pImportSemaphoreInfo->handleType != VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT)
	{
		UNSUPPORTED("pImportSemaphoreInfo->handleType %d", int(pImportSemaphoreInfo->handleType));
	}
	bool temporaryImport = (pImportSemaphoreInfo->flags & VK_SEMAPHORE_IMPORT_TEMPORARY_BIT) != 0;

	return vk::Cast(pImportSemaphoreInfo->semaphore)->importFd(pImportSemaphoreInfo->fd, temporaryImport);
}
#endif  // SWIFTSHADER_EXTERNAL_SEMAPHORE_OPAQUE_FD

#if VK_USE_PLATFORM_FUCHSIA
VKAPI_ATTR VkResult VKAPI_CALL vkImportSemaphoreZirconHandleFUCHSIA(
    VkDevice device,
    const VkImportSemaphoreZirconHandleInfoFUCHSIA *pImportSemaphoreZirconHandleInfo)
{
	TRACE("(VkDevice device = %p, const VkImportSemaphoreZirconHandleInfoFUCHSIA* pImportSemaphoreZirconHandleInfo = %p)",
	      device, pImportSemaphoreZirconHandleInfo);

	if(pImportSemaphoreZirconHandleInfo->handleType != VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TEMP_ZIRCON_EVENT_BIT_FUCHSIA)
	{
		UNSUPPORTED("pImportSemaphoreZirconHandleInfo->handleType %d", int(pImportSemaphoreZirconHandleInfo->handleType));
	}
	bool temporaryImport = (pImportSemaphoreZirconHandleInfo->flags & VK_SEMAPHORE_IMPORT_TEMPORARY_BIT) != 0;

	return vk::Cast(pImportSemaphoreZirconHandleInfo->semaphore)->importHandle(pImportSemaphoreZirconHandleInfo->handle, temporaryImport);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetSemaphoreZirconHandleFUCHSIA(
    VkDevice device,
    const VkSemaphoreGetZirconHandleInfoFUCHSIA *pGetZirconHandleInfo,
    zx_handle_t *pZirconHandle)
{
	TRACE("(VkDevice device = %p, const VkSemaphoreGetZirconHandleInfoFUCHSIA* pGetZirconHandleInfo = %p, zx_handle_t* pZirconHandle = %p)",
	      device, static_cast<const void *>(pGetZirconHandleInfo), static_cast<void *>(pZirconHandle));

	if(pGetZirconHandleInfo->handleType != VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TEMP_ZIRCON_EVENT_BIT_FUCHSIA)
	{
		UNSUPPORTED("pGetZirconHandleInfo->handleType %d", int(pGetZirconHandleInfo->handleType));
	}

	return vk::Cast(pGetZirconHandleInfo->semaphore)->exportHandle(pZirconHandle);
}
#endif  // VK_USE_PLATFORM_FUCHSIA

VKAPI_ATTR VkResult VKAPI_CALL vkCreateEvent(VkDevice device, const VkEventCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkEvent *pEvent)
{
	TRACE("(VkDevice device = %p, const VkEventCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkEvent* pEvent = %p)",
	      device, pCreateInfo, pAllocator, pEvent);

	if(pCreateInfo->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pCreateInfo->pNext);
	while(extInfo)
	{
		// Vulkan 1.2: "pNext must be NULL"
		LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
		extInfo = extInfo->pNext;
	}

	return vk::Event::Create(pAllocator, pCreateInfo, pEvent);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyEvent(VkDevice device, VkEvent event, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkEvent event = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(event), pAllocator);

	vk::destroy(event, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetEventStatus(VkDevice device, VkEvent event)
{
	TRACE("(VkDevice device = %p, VkEvent event = %p)", device, static_cast<void *>(event));

	return vk::Cast(event)->getStatus();
}

VKAPI_ATTR VkResult VKAPI_CALL vkSetEvent(VkDevice device, VkEvent event)
{
	TRACE("(VkDevice device = %p, VkEvent event = %p)", device, static_cast<void *>(event));

	vk::Cast(event)->signal();

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetEvent(VkDevice device, VkEvent event)
{
	TRACE("(VkDevice device = %p, VkEvent event = %p)", device, static_cast<void *>(event));

	vk::Cast(event)->reset();

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateQueryPool(VkDevice device, const VkQueryPoolCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkQueryPool *pQueryPool)
{
	TRACE("(VkDevice device = %p, const VkQueryPoolCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkQueryPool* pQueryPool = %p)",
	      device, pCreateInfo, pAllocator, pQueryPool);

	if(pCreateInfo->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pCreateInfo->pNext);
	while(extInfo)
	{
		LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
		extInfo = extInfo->pNext;
	}

	return vk::QueryPool::Create(pAllocator, pCreateInfo, pQueryPool);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyQueryPool(VkDevice device, VkQueryPool queryPool, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkQueryPool queryPool = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(queryPool), pAllocator);

	vk::destroy(queryPool, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetQueryPoolResults(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, size_t dataSize, void *pData, VkDeviceSize stride, VkQueryResultFlags flags)
{
	TRACE("(VkDevice device = %p, VkQueryPool queryPool = %p, uint32_t firstQuery = %d, uint32_t queryCount = %d, size_t dataSize = %d, void* pData = %p, VkDeviceSize stride = %d, VkQueryResultFlags flags = %d)",
	      device, static_cast<void *>(queryPool), int(firstQuery), int(queryCount), int(dataSize), pData, int(stride), flags);

	return vk::Cast(queryPool)->getResults(firstQuery, queryCount, dataSize, pData, stride, flags);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateBuffer(VkDevice device, const VkBufferCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkBuffer *pBuffer)
{
	TRACE("(VkDevice device = %p, const VkBufferCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkBuffer* pBuffer = %p)",
	      device, pCreateInfo, pAllocator, pBuffer);

	auto *nextInfo = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext);
	while(nextInfo)
	{
		switch(nextInfo->sType)
		{
			case VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO:
				// Do nothing. Should be handled by vk::Buffer::Create().
				break;
			default:
				LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(nextInfo->sType).c_str());
				break;
		}
		nextInfo = nextInfo->pNext;
	}

	return vk::Buffer::Create(pAllocator, pCreateInfo, pBuffer);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyBuffer(VkDevice device, VkBuffer buffer, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkBuffer buffer = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(buffer), pAllocator);

	vk::destroy(buffer, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateBufferView(VkDevice device, const VkBufferViewCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkBufferView *pView)
{
	TRACE("(VkDevice device = %p, const VkBufferViewCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkBufferView* pView = %p)",
	      device, pCreateInfo, pAllocator, pView);

	if(pCreateInfo->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pCreateInfo->pNext);
	while(extInfo)
	{
		LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
		extInfo = extInfo->pNext;
	}

	return vk::BufferView::Create(pAllocator, pCreateInfo, pView);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyBufferView(VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkBufferView bufferView = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(bufferView), pAllocator);

	vk::destroy(bufferView, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateImage(VkDevice device, const VkImageCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkImage *pImage)
{
	TRACE("(VkDevice device = %p, const VkImageCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkImage* pImage = %p)",
	      device, pCreateInfo, pAllocator, pImage);

	const VkBaseInStructure *extensionCreateInfo = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext);

#ifdef __ANDROID__
	vk::BackingMemory backmem;
	bool swapchainImage = false;
#endif

	while(extensionCreateInfo)
	{
		switch((long)(extensionCreateInfo->sType))
		{
#ifdef __ANDROID__
			case VK_STRUCTURE_TYPE_SWAPCHAIN_IMAGE_CREATE_INFO_ANDROID:
			{
				const VkSwapchainImageCreateInfoANDROID *swapImageCreateInfo = reinterpret_cast<const VkSwapchainImageCreateInfoANDROID *>(extensionCreateInfo);
				backmem.androidUsage = swapImageCreateInfo->usage;
			}
			break;
			case VK_STRUCTURE_TYPE_NATIVE_BUFFER_ANDROID:
			{
				const VkNativeBufferANDROID *nativeBufferInfo = reinterpret_cast<const VkNativeBufferANDROID *>(extensionCreateInfo);
				backmem.nativeHandle = nativeBufferInfo->handle;
				backmem.stride = nativeBufferInfo->stride;
				swapchainImage = true;
			}
			break;
#endif
			case VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO:
				// Do nothing. Should be handled by vk::Image::Create()
				break;
			case VK_STRUCTURE_TYPE_IMAGE_SWAPCHAIN_CREATE_INFO_KHR:
				/* Do nothing. We don't actually need the swapchain handle yet; we'll do all the work in vkBindImageMemory2. */
				break;
			default:
				// "the [driver] must skip over, without processing (other than reading the sType and pNext members) any structures in the chain with sType values not defined by [supported extenions]"
				LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(extensionCreateInfo->sType).c_str());
				break;
		}

		extensionCreateInfo = extensionCreateInfo->pNext;
	}

	VkResult result = vk::Image::Create(pAllocator, pCreateInfo, pImage, vk::Cast(device));

#ifdef __ANDROID__
	if(swapchainImage)
	{
		if(result != VK_SUCCESS)
		{
			return result;
		}

		vk::Image *image = vk::Cast(*pImage);
		VkMemoryRequirements memRequirements = image->getMemoryRequirements();

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = 0;

		VkDeviceMemory devmem = { VK_NULL_HANDLE };
		result = vkAllocateMemory(device, &allocInfo, pAllocator, &devmem);
		if(result != VK_SUCCESS)
		{
			return result;
		}

		vkBindImageMemory(device, *pImage, devmem, 0);
		backmem.externalMemory = true;

		image->setBackingMemory(backmem);
	}
#endif

	return result;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkImage image = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(image), pAllocator);

#ifdef __ANDROID__
	vk::Image *img = vk::Cast(image);
	if(img && img->hasExternalMemory())
	{
		vk::destroy(img->getExternalMemory(), pAllocator);
	}
#endif

	vk::destroy(image, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL vkGetImageSubresourceLayout(VkDevice device, VkImage image, const VkImageSubresource *pSubresource, VkSubresourceLayout *pLayout)
{
	TRACE("(VkDevice device = %p, VkImage image = %p, const VkImageSubresource* pSubresource = %p, VkSubresourceLayout* pLayout = %p)",
	      device, static_cast<void *>(image), pSubresource, pLayout);

	vk::Cast(image)->getSubresourceLayout(pSubresource, pLayout);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateImageView(VkDevice device, const VkImageViewCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkImageView *pView)
{
	TRACE("(VkDevice device = %p, const VkImageViewCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkImageView* pView = %p)",
	      device, pCreateInfo, pAllocator, pView);

	if(pCreateInfo->flags != 0)
	{
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	const VkBaseInStructure *extensionCreateInfo = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext);
	const vk::SamplerYcbcrConversion *ycbcrConversion = nullptr;

	while(extensionCreateInfo)
	{
		switch(extensionCreateInfo->sType)
		{
			case VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO_KHR:
			{
				const VkImageViewUsageCreateInfo *multiviewCreateInfo = reinterpret_cast<const VkImageViewUsageCreateInfo *>(extensionCreateInfo);
				ASSERT(!(~vk::Cast(pCreateInfo->image)->getUsage() & multiviewCreateInfo->usage));
			}
			break;
			case VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO:
			{
				const VkSamplerYcbcrConversionInfo *samplerYcbcrConversionInfo = reinterpret_cast<const VkSamplerYcbcrConversionInfo *>(extensionCreateInfo);
				ycbcrConversion = vk::Cast(samplerYcbcrConversionInfo->conversion);

				if(ycbcrConversion)
				{
					ASSERT((pCreateInfo->components.r == VK_COMPONENT_SWIZZLE_IDENTITY) &&
					       (pCreateInfo->components.g == VK_COMPONENT_SWIZZLE_IDENTITY) &&
					       (pCreateInfo->components.b == VK_COMPONENT_SWIZZLE_IDENTITY) &&
					       (pCreateInfo->components.a == VK_COMPONENT_SWIZZLE_IDENTITY));
				}
			}
			break;
			default:
				LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(extensionCreateInfo->sType).c_str());
				break;
		}

		extensionCreateInfo = extensionCreateInfo->pNext;
	}

	return vk::ImageView::Create(pAllocator, pCreateInfo, pView, ycbcrConversion);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyImageView(VkDevice device, VkImageView imageView, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkImageView imageView = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(imageView), pAllocator);

	vk::destroy(imageView, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkShaderModule *pShaderModule)
{
	TRACE("(VkDevice device = %p, const VkShaderModuleCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkShaderModule* pShaderModule = %p)",
	      device, pCreateInfo, pAllocator, pShaderModule);

	if(pCreateInfo->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	auto *nextInfo = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext);
	while(nextInfo)
	{
		LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(nextInfo->sType).c_str());
		nextInfo = nextInfo->pNext;
	}

	return vk::ShaderModule::Create(pAllocator, pCreateInfo, pShaderModule);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkShaderModule shaderModule = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(shaderModule), pAllocator);

	vk::destroy(shaderModule, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkPipelineCache *pPipelineCache)
{
	TRACE("(VkDevice device = %p, const VkPipelineCacheCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkPipelineCache* pPipelineCache = %p)",
	      device, pCreateInfo, pAllocator, pPipelineCache);

	if(pCreateInfo->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pCreateInfo->pNext);
	while(extInfo)
	{
		LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
		extInfo = extInfo->pNext;
	}

	return vk::PipelineCache::Create(pAllocator, pCreateInfo, pPipelineCache);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkPipelineCache pipelineCache = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(pipelineCache), pAllocator);

	vk::destroy(pipelineCache, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineCacheData(VkDevice device, VkPipelineCache pipelineCache, size_t *pDataSize, void *pData)
{
	TRACE("(VkDevice device = %p, VkPipelineCache pipelineCache = %p, size_t* pDataSize = %p, void* pData = %p)",
	      device, static_cast<void *>(pipelineCache), pDataSize, pData);

	return vk::Cast(pipelineCache)->getData(pDataSize, pData);
}

VKAPI_ATTR VkResult VKAPI_CALL vkMergePipelineCaches(VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount, const VkPipelineCache *pSrcCaches)
{
	TRACE("(VkDevice device = %p, VkPipelineCache dstCache = %p, uint32_t srcCacheCount = %d, const VkPipelineCache* pSrcCaches = %p)",
	      device, static_cast<void *>(dstCache), int(srcCacheCount), pSrcCaches);

	return vk::Cast(dstCache)->merge(srcCacheCount, pSrcCaches);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo *pCreateInfos, const VkAllocationCallbacks *pAllocator, VkPipeline *pPipelines)
{
	TRACE("(VkDevice device = %p, VkPipelineCache pipelineCache = %p, uint32_t createInfoCount = %d, const VkGraphicsPipelineCreateInfo* pCreateInfos = %p, const VkAllocationCallbacks* pAllocator = %p, VkPipeline* pPipelines = %p)",
	      device, static_cast<void *>(pipelineCache), int(createInfoCount), pCreateInfos, pAllocator, pPipelines);

	VkResult errorResult = VK_SUCCESS;
	for(uint32_t i = 0; i < createInfoCount; i++)
	{
		VkResult result = vk::GraphicsPipeline::Create(pAllocator, &pCreateInfos[i], &pPipelines[i], vk::Cast(device));

		if(result == VK_SUCCESS)
		{
			static_cast<vk::GraphicsPipeline *>(vk::Cast(pPipelines[i]))->compileShaders(pAllocator, &pCreateInfos[i], vk::Cast(pipelineCache));
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

VKAPI_ATTR VkResult VKAPI_CALL vkCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkComputePipelineCreateInfo *pCreateInfos, const VkAllocationCallbacks *pAllocator, VkPipeline *pPipelines)
{
	TRACE("(VkDevice device = %p, VkPipelineCache pipelineCache = %p, uint32_t createInfoCount = %d, const VkComputePipelineCreateInfo* pCreateInfos = %p, const VkAllocationCallbacks* pAllocator = %p, VkPipeline* pPipelines = %p)",
	      device, static_cast<void *>(pipelineCache), int(createInfoCount), pCreateInfos, pAllocator, pPipelines);

	VkResult errorResult = VK_SUCCESS;
	for(uint32_t i = 0; i < createInfoCount; i++)
	{
		VkResult result = vk::ComputePipeline::Create(pAllocator, &pCreateInfos[i], &pPipelines[i], vk::Cast(device));

		if(result == VK_SUCCESS)
		{
			static_cast<vk::ComputePipeline *>(vk::Cast(pPipelines[i]))->compileShaders(pAllocator, &pCreateInfos[i], vk::Cast(pipelineCache));
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

VKAPI_ATTR void VKAPI_CALL vkDestroyPipeline(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkPipeline pipeline = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(pipeline), pAllocator);

	vk::destroy(pipeline, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkPipelineLayout *pPipelineLayout)
{
	TRACE("(VkDevice device = %p, const VkPipelineLayoutCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkPipelineLayout* pPipelineLayout = %p)",
	      device, pCreateInfo, pAllocator, pPipelineLayout);

	if(pCreateInfo->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	auto *nextInfo = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext);
	while(nextInfo)
	{
		LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(nextInfo->sType).c_str());
		nextInfo = nextInfo->pNext;
	}

	return vk::PipelineLayout::Create(pAllocator, pCreateInfo, pPipelineLayout);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkPipelineLayout pipelineLayout = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(pipelineLayout), pAllocator);

	vk::destroy(pipelineLayout, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateSampler(VkDevice device, const VkSamplerCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSampler *pSampler)
{
	TRACE("(VkDevice device = %p, const VkSamplerCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkSampler* pSampler = %p)",
	      device, pCreateInfo, pAllocator, pSampler);

	if(pCreateInfo->flags != 0)
	{
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	const VkBaseInStructure *extensionCreateInfo = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext);
	const vk::SamplerYcbcrConversion *ycbcrConversion = nullptr;

	while(extensionCreateInfo)
	{
		switch(extensionCreateInfo->sType)
		{
			case VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO:
			{
				const VkSamplerYcbcrConversionInfo *samplerYcbcrConversionInfo = reinterpret_cast<const VkSamplerYcbcrConversionInfo *>(extensionCreateInfo);
				ycbcrConversion = vk::Cast(samplerYcbcrConversionInfo->conversion);
			}
			break;
			default:
				LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(extensionCreateInfo->sType).c_str());
				break;
		}

		extensionCreateInfo = extensionCreateInfo->pNext;
	}

	vk::SamplerState samplerState(pCreateInfo, ycbcrConversion);
	uint32_t samplerID = vk::Cast(device)->indexSampler(samplerState);

	VkResult result = vk::Sampler::Create(pAllocator, pCreateInfo, pSampler, samplerState, samplerID);

	if(*pSampler == VK_NULL_HANDLE)
	{
		ASSERT(result != VK_SUCCESS);
		vk::Cast(device)->removeSampler(samplerState);
	}

	return result;
}

VKAPI_ATTR void VKAPI_CALL vkDestroySampler(VkDevice device, VkSampler sampler, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkSampler sampler = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(sampler), pAllocator);

	if(sampler != VK_NULL_HANDLE)
	{
		vk::Cast(device)->removeSampler(*vk::Cast(sampler));

		vk::destroy(sampler, pAllocator);
	}
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDescriptorSetLayout *pSetLayout)
{
	TRACE("(VkDevice device = %p, const VkDescriptorSetLayoutCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkDescriptorSetLayout* pSetLayout = %p)",
	      device, pCreateInfo, pAllocator, pSetLayout);

	const VkBaseInStructure *extensionCreateInfo = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext);

	while(extensionCreateInfo)
	{
		switch(extensionCreateInfo->sType)
		{
			case VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT:
				ASSERT(!vk::Cast(device)->hasExtension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME));
				break;
			default:
				LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(extensionCreateInfo->sType).c_str());
				break;
		}

		extensionCreateInfo = extensionCreateInfo->pNext;
	}

	return vk::DescriptorSetLayout::Create(pAllocator, pCreateInfo, pSetLayout);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkDescriptorSetLayout descriptorSetLayout = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(descriptorSetLayout), pAllocator);

	vk::destroy(descriptorSetLayout, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDescriptorPool *pDescriptorPool)
{
	TRACE("(VkDevice device = %p, const VkDescriptorPoolCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkDescriptorPool* pDescriptorPool = %p)",
	      device, pCreateInfo, pAllocator, pDescriptorPool);

	auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pCreateInfo->pNext);
	while(extInfo)
	{
		LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
		extInfo = extInfo->pNext;
	}

	return vk::DescriptorPool::Create(pAllocator, pCreateInfo, pDescriptorPool);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkDescriptorPool descriptorPool = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(descriptorPool), pAllocator);

	vk::destroy(descriptorPool, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags)
{
	TRACE("(VkDevice device = %p, VkDescriptorPool descriptorPool = %p, VkDescriptorPoolResetFlags flags = 0x%x)",
	      device, static_cast<void *>(descriptorPool), int(flags));

	if(flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("flags %d", int(flags));
	}

	return vk::Cast(descriptorPool)->reset();
}

VKAPI_ATTR VkResult VKAPI_CALL vkAllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo *pAllocateInfo, VkDescriptorSet *pDescriptorSets)
{
	TRACE("(VkDevice device = %p, const VkDescriptorSetAllocateInfo* pAllocateInfo = %p, VkDescriptorSet* pDescriptorSets = %p)",
	      device, pAllocateInfo, pDescriptorSets);

	auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pAllocateInfo->pNext);
	while(extInfo)
	{
		LOG_TRAP("pAllocateInfo->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
		extInfo = extInfo->pNext;
	}

	return vk::Cast(pAllocateInfo->descriptorPool)->allocateSets(pAllocateInfo->descriptorSetCount, pAllocateInfo->pSetLayouts, pDescriptorSets);
}

VKAPI_ATTR VkResult VKAPI_CALL vkFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet *pDescriptorSets)
{
	TRACE("(VkDevice device = %p, VkDescriptorPool descriptorPool = %p, uint32_t descriptorSetCount = %d, const VkDescriptorSet* pDescriptorSets = %p)",
	      device, static_cast<void *>(descriptorPool), descriptorSetCount, pDescriptorSets);

	vk::Cast(descriptorPool)->freeSets(descriptorSetCount, pDescriptorSets);

	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkUpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount, const VkWriteDescriptorSet *pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet *pDescriptorCopies)
{
	TRACE("(VkDevice device = %p, uint32_t descriptorWriteCount = %d, const VkWriteDescriptorSet* pDescriptorWrites = %p, uint32_t descriptorCopyCount = %d, const VkCopyDescriptorSet* pDescriptorCopies = %p)",
	      device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);

	vk::Cast(device)->updateDescriptorSets(descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateFramebuffer(VkDevice device, const VkFramebufferCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkFramebuffer *pFramebuffer)
{
	TRACE("(VkDevice device = %p, const VkFramebufferCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkFramebuffer* pFramebuffer = %p)",
	      device, pCreateInfo, pAllocator, pFramebuffer);

	if(pCreateInfo->flags != 0)
	{
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	auto *nextInfo = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext);
	while(nextInfo)
	{
		LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(nextInfo->sType).c_str());
		nextInfo = nextInfo->pNext;
	}

	return vk::Framebuffer::Create(pAllocator, pCreateInfo, pFramebuffer);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkFramebuffer framebuffer = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(framebuffer), pAllocator);

	vk::destroy(framebuffer, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateRenderPass(VkDevice device, const VkRenderPassCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkRenderPass *pRenderPass)
{
	TRACE("(VkDevice device = %p, const VkRenderPassCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkRenderPass* pRenderPass = %p)",
	      device, pCreateInfo, pAllocator, pRenderPass);

	if(pCreateInfo->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	ValidateRenderPassPNextChain(device, pCreateInfo);

	return vk::RenderPass::Create(pAllocator, pCreateInfo, pRenderPass);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateRenderPass2KHR(VkDevice device, const VkRenderPassCreateInfo2KHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkRenderPass *pRenderPass)
{
	TRACE("(VkDevice device = %p, const VkRenderPassCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkRenderPass* pRenderPass = %p)",
	      device, pCreateInfo, pAllocator, pRenderPass);

	if(pCreateInfo->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	ValidateRenderPassPNextChain(device, pCreateInfo);

	return vk::RenderPass::Create(pAllocator, pCreateInfo, pRenderPass);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkRenderPass renderPass = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(renderPass), pAllocator);

	vk::destroy(renderPass, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL vkGetRenderAreaGranularity(VkDevice device, VkRenderPass renderPass, VkExtent2D *pGranularity)
{
	TRACE("(VkDevice device = %p, VkRenderPass renderPass = %p, VkExtent2D* pGranularity = %p)",
	      device, static_cast<void *>(renderPass), pGranularity);

	vk::Cast(renderPass)->getRenderAreaGranularity(pGranularity);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkCommandPool *pCommandPool)
{
	TRACE("(VkDevice device = %p, const VkCommandPoolCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkCommandPool* pCommandPool = %p)",
	      device, pCreateInfo, pAllocator, pCommandPool);

	auto *nextInfo = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext);
	while(nextInfo)
	{
		LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(nextInfo->sType).c_str());
		nextInfo = nextInfo->pNext;
	}

	return vk::CommandPool::Create(pAllocator, pCreateInfo, pCommandPool);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkCommandPool commandPool = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(commandPool), pAllocator);

	vk::destroy(commandPool, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags)
{
	TRACE("(VkDevice device = %p, VkCommandPool commandPool = %p, VkCommandPoolResetFlags flags = %d)",
	      device, static_cast<void *>(commandPool), int(flags));

	return vk::Cast(commandPool)->reset(flags);
}

VKAPI_ATTR VkResult VKAPI_CALL vkAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo *pAllocateInfo, VkCommandBuffer *pCommandBuffers)
{
	TRACE("(VkDevice device = %p, const VkCommandBufferAllocateInfo* pAllocateInfo = %p, VkCommandBuffer* pCommandBuffers = %p)",
	      device, pAllocateInfo, pCommandBuffers);

	auto *nextInfo = reinterpret_cast<const VkBaseInStructure *>(pAllocateInfo->pNext);
	while(nextInfo)
	{
		LOG_TRAP("pAllocateInfo->pNext sType = %s", vk::Stringify(nextInfo->sType).c_str());
		nextInfo = nextInfo->pNext;
	}

	return vk::Cast(pAllocateInfo->commandPool)->allocateCommandBuffers(vk::Cast(device), pAllocateInfo->level, pAllocateInfo->commandBufferCount, pCommandBuffers);
}

VKAPI_ATTR void VKAPI_CALL vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer *pCommandBuffers)
{
	TRACE("(VkDevice device = %p, VkCommandPool commandPool = %p, uint32_t commandBufferCount = %d, const VkCommandBuffer* pCommandBuffers = %p)",
	      device, static_cast<void *>(commandPool), int(commandBufferCount), pCommandBuffers);

	vk::Cast(commandPool)->freeCommandBuffers(commandBufferCount, pCommandBuffers);
}

VKAPI_ATTR VkResult VKAPI_CALL vkBeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo *pBeginInfo)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, const VkCommandBufferBeginInfo* pBeginInfo = %p)",
	      commandBuffer, pBeginInfo);

	auto *nextInfo = reinterpret_cast<const VkBaseInStructure *>(pBeginInfo->pNext);
	while(nextInfo)
	{
		LOG_TRAP("pBeginInfo->pNext sType = %s", vk::Stringify(nextInfo->sType).c_str());
		nextInfo = nextInfo->pNext;
	}

	return vk::Cast(commandBuffer)->begin(pBeginInfo->flags, pBeginInfo->pInheritanceInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEndCommandBuffer(VkCommandBuffer commandBuffer)
{
	TRACE("(VkCommandBuffer commandBuffer = %p)", commandBuffer);

	return vk::Cast(commandBuffer)->end();
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags)
{
	TRACE("VkCommandBuffer commandBuffer = %p, VkCommandBufferResetFlags flags = %d", commandBuffer, int(flags));

	return vk::Cast(commandBuffer)->reset(flags);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkPipelineBindPoint pipelineBindPoint = %d, VkPipeline pipeline = %p)",
	      commandBuffer, int(pipelineBindPoint), static_cast<void *>(pipeline));

	vk::Cast(commandBuffer)->bindPipeline(pipelineBindPoint, vk::Cast(pipeline));
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport *pViewports)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, uint32_t firstViewport = %d, uint32_t viewportCount = %d, const VkViewport* pViewports = %p)",
	      commandBuffer, int(firstViewport), int(viewportCount), pViewports);

	vk::Cast(commandBuffer)->setViewport(firstViewport, viewportCount, pViewports);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D *pScissors)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, uint32_t firstScissor = %d, uint32_t scissorCount = %d, const VkRect2D* pScissors = %p)",
	      commandBuffer, int(firstScissor), int(scissorCount), pScissors);

	vk::Cast(commandBuffer)->setScissor(firstScissor, scissorCount, pScissors);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, float lineWidth = %f)", commandBuffer, lineWidth);

	vk::Cast(commandBuffer)->setLineWidth(lineWidth);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBias(VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, float depthBiasConstantFactor = %f, float depthBiasClamp = %f, float depthBiasSlopeFactor = %f)",
	      commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);

	vk::Cast(commandBuffer)->setDepthBias(depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetBlendConstants(VkCommandBuffer commandBuffer, const float blendConstants[4])
{
	TRACE("(VkCommandBuffer commandBuffer = %p, const float blendConstants[4] = {%f, %f, %f, %f})",
	      commandBuffer, blendConstants[0], blendConstants[1], blendConstants[2], blendConstants[3]);

	vk::Cast(commandBuffer)->setBlendConstants(blendConstants);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBounds(VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, float minDepthBounds = %f, float maxDepthBounds = %f)",
	      commandBuffer, minDepthBounds, maxDepthBounds);

	vk::Cast(commandBuffer)->setDepthBounds(minDepthBounds, maxDepthBounds);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilCompareMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t compareMask)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkStencilFaceFlags faceMask = %d, uint32_t compareMask = %d)",
	      commandBuffer, int(faceMask), int(compareMask));

	vk::Cast(commandBuffer)->setStencilCompareMask(faceMask, compareMask);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilWriteMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t writeMask)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkStencilFaceFlags faceMask = %d, uint32_t writeMask = %d)",
	      commandBuffer, int(faceMask), int(writeMask));

	vk::Cast(commandBuffer)->setStencilWriteMask(faceMask, writeMask);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilReference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t reference)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkStencilFaceFlags faceMask = %d, uint32_t reference = %d)",
	      commandBuffer, int(faceMask), int(reference));

	vk::Cast(commandBuffer)->setStencilReference(faceMask, reference);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet *pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t *pDynamicOffsets)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkPipelineBindPoint pipelineBindPoint = %d, VkPipelineLayout layout = %p, uint32_t firstSet = %d, uint32_t descriptorSetCount = %d, const VkDescriptorSet* pDescriptorSets = %p, uint32_t dynamicOffsetCount = %d, const uint32_t* pDynamicOffsets = %p)",
	      commandBuffer, int(pipelineBindPoint), static_cast<void *>(layout), int(firstSet), int(descriptorSetCount), pDescriptorSets, int(dynamicOffsetCount), pDynamicOffsets);

	vk::Cast(commandBuffer)->bindDescriptorSets(pipelineBindPoint, vk::Cast(layout), firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkBuffer buffer = %p, VkDeviceSize offset = %d, VkIndexType indexType = %d)",
	      commandBuffer, static_cast<void *>(buffer), int(offset), int(indexType));

	vk::Cast(commandBuffer)->bindIndexBuffer(vk::Cast(buffer), offset, indexType);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer *pBuffers, const VkDeviceSize *pOffsets)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, uint32_t firstBinding = %d, uint32_t bindingCount = %d, const VkBuffer* pBuffers = %p, const VkDeviceSize* pOffsets = %p)",
	      commandBuffer, int(firstBinding), int(bindingCount), pBuffers, pOffsets);

	vk::Cast(commandBuffer)->bindVertexBuffers(firstBinding, bindingCount, pBuffers, pOffsets);
}

VKAPI_ATTR void VKAPI_CALL vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, uint32_t vertexCount = %d, uint32_t instanceCount = %d, uint32_t firstVertex = %d, uint32_t firstInstance = %d)",
	      commandBuffer, int(vertexCount), int(instanceCount), int(firstVertex), int(firstInstance));

	vk::Cast(commandBuffer)->draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, uint32_t indexCount = %d, uint32_t instanceCount = %d, uint32_t firstIndex = %d, int32_t vertexOffset = %d, uint32_t firstInstance = %d)",
	      commandBuffer, int(indexCount), int(instanceCount), int(firstIndex), int(vertexOffset), int(firstInstance));

	vk::Cast(commandBuffer)->drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkBuffer buffer = %p, VkDeviceSize offset = %d, uint32_t drawCount = %d, uint32_t stride = %d)",
	      commandBuffer, static_cast<void *>(buffer), int(offset), int(drawCount), int(stride));

	vk::Cast(commandBuffer)->drawIndirect(vk::Cast(buffer), offset, drawCount, stride);
}

VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkBuffer buffer = %p, VkDeviceSize offset = %d, uint32_t drawCount = %d, uint32_t stride = %d)",
	      commandBuffer, static_cast<void *>(buffer), int(offset), int(drawCount), int(stride));

	vk::Cast(commandBuffer)->drawIndexedIndirect(vk::Cast(buffer), offset, drawCount, stride);
}

VKAPI_ATTR void VKAPI_CALL vkCmdDispatch(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, uint32_t groupCountX = %d, uint32_t groupCountY = %d, uint32_t groupCountZ = %d)",
	      commandBuffer, int(groupCountX), int(groupCountY), int(groupCountZ));

	vk::Cast(commandBuffer)->dispatch(groupCountX, groupCountY, groupCountZ);
}

VKAPI_ATTR void VKAPI_CALL vkCmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkBuffer buffer = %p, VkDeviceSize offset = %d)",
	      commandBuffer, static_cast<void *>(buffer), int(offset));

	vk::Cast(commandBuffer)->dispatchIndirect(vk::Cast(buffer), offset);
}

VKAPI_ATTR void VKAPI_CALL vkCmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy *pRegions)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkBuffer srcBuffer = %p, VkBuffer dstBuffer = %p, uint32_t regionCount = %d, const VkBufferCopy* pRegions = %p)",
	      commandBuffer, static_cast<void *>(srcBuffer), static_cast<void *>(dstBuffer), int(regionCount), pRegions);

	vk::Cast(commandBuffer)->copyBuffer(vk::Cast(srcBuffer), vk::Cast(dstBuffer), regionCount, pRegions);
}

VKAPI_ATTR void VKAPI_CALL vkCmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy *pRegions)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkImage srcImage = %p, VkImageLayout srcImageLayout = %d, VkImage dstImage = %p, VkImageLayout dstImageLayout = %d, uint32_t regionCount = %d, const VkImageCopy* pRegions = %p)",
	      commandBuffer, static_cast<void *>(srcImage), srcImageLayout, static_cast<void *>(dstImage), dstImageLayout, int(regionCount), pRegions);

	vk::Cast(commandBuffer)->copyImage(vk::Cast(srcImage), srcImageLayout, vk::Cast(dstImage), dstImageLayout, regionCount, pRegions);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit *pRegions, VkFilter filter)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkImage srcImage = %p, VkImageLayout srcImageLayout = %d, VkImage dstImage = %p, VkImageLayout dstImageLayout = %d, uint32_t regionCount = %d, const VkImageBlit* pRegions = %p, VkFilter filter = %d)",
	      commandBuffer, static_cast<void *>(srcImage), srcImageLayout, static_cast<void *>(dstImage), dstImageLayout, int(regionCount), pRegions, filter);

	vk::Cast(commandBuffer)->blitImage(vk::Cast(srcImage), srcImageLayout, vk::Cast(dstImage), dstImageLayout, regionCount, pRegions, filter);
}

VKAPI_ATTR void VKAPI_CALL vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy *pRegions)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkBuffer srcBuffer = %p, VkImage dstImage = %p, VkImageLayout dstImageLayout = %d, uint32_t regionCount = %d, const VkBufferImageCopy* pRegions = %p)",
	      commandBuffer, static_cast<void *>(srcBuffer), static_cast<void *>(dstImage), dstImageLayout, int(regionCount), pRegions);

	vk::Cast(commandBuffer)->copyBufferToImage(vk::Cast(srcBuffer), vk::Cast(dstImage), dstImageLayout, regionCount, pRegions);
}

VKAPI_ATTR void VKAPI_CALL vkCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy *pRegions)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkImage srcImage = %p, VkImageLayout srcImageLayout = %d, VkBuffer dstBuffer = %p, uint32_t regionCount = %d, const VkBufferImageCopy* pRegions = %p)",
	      commandBuffer, static_cast<void *>(srcImage), int(srcImageLayout), static_cast<void *>(dstBuffer), int(regionCount), pRegions);

	vk::Cast(commandBuffer)->copyImageToBuffer(vk::Cast(srcImage), srcImageLayout, vk::Cast(dstBuffer), regionCount, pRegions);
}

VKAPI_ATTR void VKAPI_CALL vkCmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void *pData)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkBuffer dstBuffer = %p, VkDeviceSize dstOffset = %d, VkDeviceSize dataSize = %d, const void* pData = %p)",
	      commandBuffer, static_cast<void *>(dstBuffer), int(dstOffset), int(dataSize), pData);

	vk::Cast(commandBuffer)->updateBuffer(vk::Cast(dstBuffer), dstOffset, dataSize, pData);
}

VKAPI_ATTR void VKAPI_CALL vkCmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkBuffer dstBuffer = %p, VkDeviceSize dstOffset = %d, VkDeviceSize size = %d, uint32_t data = %d)",
	      commandBuffer, static_cast<void *>(dstBuffer), int(dstOffset), int(size), data);

	vk::Cast(commandBuffer)->fillBuffer(vk::Cast(dstBuffer), dstOffset, size, data);
}

VKAPI_ATTR void VKAPI_CALL vkCmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue *pColor, uint32_t rangeCount, const VkImageSubresourceRange *pRanges)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkImage image = %p, VkImageLayout imageLayout = %d, const VkClearColorValue* pColor = %p, uint32_t rangeCount = %d, const VkImageSubresourceRange* pRanges = %p)",
	      commandBuffer, static_cast<void *>(image), int(imageLayout), pColor, int(rangeCount), pRanges);

	vk::Cast(commandBuffer)->clearColorImage(vk::Cast(image), imageLayout, pColor, rangeCount, pRanges);
}

VKAPI_ATTR void VKAPI_CALL vkCmdClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue *pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange *pRanges)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkImage image = %p, VkImageLayout imageLayout = %d, const VkClearDepthStencilValue* pDepthStencil = %p, uint32_t rangeCount = %d, const VkImageSubresourceRange* pRanges = %p)",
	      commandBuffer, static_cast<void *>(image), int(imageLayout), pDepthStencil, int(rangeCount), pRanges);

	vk::Cast(commandBuffer)->clearDepthStencilImage(vk::Cast(image), imageLayout, pDepthStencil, rangeCount, pRanges);
}

VKAPI_ATTR void VKAPI_CALL vkCmdClearAttachments(VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkClearAttachment *pAttachments, uint32_t rectCount, const VkClearRect *pRects)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, uint32_t attachmentCount = %d, const VkClearAttachment* pAttachments = %p, uint32_t rectCount = %d, const VkClearRect* pRects = %p)",
	      commandBuffer, int(attachmentCount), pAttachments, int(rectCount), pRects);

	vk::Cast(commandBuffer)->clearAttachments(attachmentCount, pAttachments, rectCount, pRects);
}

VKAPI_ATTR void VKAPI_CALL vkCmdResolveImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve *pRegions)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkImage srcImage = %p, VkImageLayout srcImageLayout = %d, VkImage dstImage = %p, VkImageLayout dstImageLayout = %d, uint32_t regionCount = %d, const VkImageResolve* pRegions = %p)",
	      commandBuffer, static_cast<void *>(srcImage), int(srcImageLayout), static_cast<void *>(dstImage), int(dstImageLayout), regionCount, pRegions);

	vk::Cast(commandBuffer)->resolveImage(vk::Cast(srcImage), srcImageLayout, vk::Cast(dstImage), dstImageLayout, regionCount, pRegions);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkEvent event = %p, VkPipelineStageFlags stageMask = %d)",
	      commandBuffer, static_cast<void *>(event), int(stageMask));

	vk::Cast(commandBuffer)->setEvent(vk::Cast(event), stageMask);
}

VKAPI_ATTR void VKAPI_CALL vkCmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkEvent event = %p, VkPipelineStageFlags stageMask = %d)",
	      commandBuffer, static_cast<void *>(event), int(stageMask));

	vk::Cast(commandBuffer)->resetEvent(vk::Cast(event), stageMask);
}

VKAPI_ATTR void VKAPI_CALL vkCmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent *pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier *pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier *pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier *pImageMemoryBarriers)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, uint32_t eventCount = %d, const VkEvent* pEvents = %p, VkPipelineStageFlags srcStageMask = 0x%x, VkPipelineStageFlags dstStageMask = 0x%x, uint32_t memoryBarrierCount = %d, const VkMemoryBarrier* pMemoryBarriers = %p, uint32_t bufferMemoryBarrierCount = %d, const VkBufferMemoryBarrier* pBufferMemoryBarriers = %p, uint32_t imageMemoryBarrierCount = %d, const VkImageMemoryBarrier* pImageMemoryBarriers = %p)",
	      commandBuffer, int(eventCount), pEvents, int(srcStageMask), int(dstStageMask), int(memoryBarrierCount), pMemoryBarriers, int(bufferMemoryBarrierCount), pBufferMemoryBarriers, int(imageMemoryBarrierCount), pImageMemoryBarriers);

	vk::Cast(commandBuffer)->waitEvents(eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

VKAPI_ATTR void VKAPI_CALL vkCmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier *pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier *pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier *pImageMemoryBarriers)
{
	TRACE(
	    "(VkCommandBuffer commandBuffer = %p, VkPipelineStageFlags srcStageMask = 0x%x, VkPipelineStageFlags dstStageMask = 0x%x, VkDependencyFlags dependencyFlags = %d, uint32_t memoryBarrierCount = %d, onst VkMemoryBarrier* pMemoryBarriers = %p,"
	    " uint32_t bufferMemoryBarrierCount = %d, const VkBufferMemoryBarrier* pBufferMemoryBarriers = %p, uint32_t imageMemoryBarrierCount = %d, const VkImageMemoryBarrier* pImageMemoryBarriers = %p)",
	    commandBuffer, int(srcStageMask), int(dstStageMask), dependencyFlags, int(memoryBarrierCount), pMemoryBarriers, int(bufferMemoryBarrierCount), pBufferMemoryBarriers, int(imageMemoryBarrierCount), pImageMemoryBarriers);

	vk::Cast(commandBuffer)->pipelineBarrier(srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkQueryPool queryPool = %p, uint32_t query = %d, VkQueryControlFlags flags = %d)",
	      commandBuffer, static_cast<void *>(queryPool), query, int(flags));

	vk::Cast(commandBuffer)->beginQuery(vk::Cast(queryPool), query, flags);
}

VKAPI_ATTR void VKAPI_CALL vkCmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkQueryPool queryPool = %p, uint32_t query = %d)",
	      commandBuffer, static_cast<void *>(queryPool), int(query));

	vk::Cast(commandBuffer)->endQuery(vk::Cast(queryPool), query);
}

VKAPI_ATTR void VKAPI_CALL vkCmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkQueryPool queryPool = %p, uint32_t firstQuery = %d, uint32_t queryCount = %d)",
	      commandBuffer, static_cast<void *>(queryPool), int(firstQuery), int(queryCount));

	vk::Cast(commandBuffer)->resetQueryPool(vk::Cast(queryPool), firstQuery, queryCount);
}

VKAPI_ATTR void VKAPI_CALL vkCmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkPipelineStageFlagBits pipelineStage = %d, VkQueryPool queryPool = %p, uint32_t query = %d)",
	      commandBuffer, int(pipelineStage), static_cast<void *>(queryPool), int(query));

	vk::Cast(commandBuffer)->writeTimestamp(pipelineStage, vk::Cast(queryPool), query);
}

VKAPI_ATTR void VKAPI_CALL vkCmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkQueryPool queryPool = %p, uint32_t firstQuery = %d, uint32_t queryCount = %d, VkBuffer dstBuffer = %p, VkDeviceSize dstOffset = %d, VkDeviceSize stride = %d, VkQueryResultFlags flags = %d)",
	      commandBuffer, static_cast<void *>(queryPool), int(firstQuery), int(queryCount), static_cast<void *>(dstBuffer), int(dstOffset), int(stride), int(flags));

	vk::Cast(commandBuffer)->copyQueryPoolResults(vk::Cast(queryPool), firstQuery, queryCount, vk::Cast(dstBuffer), dstOffset, stride, flags);
}

VKAPI_ATTR void VKAPI_CALL vkCmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void *pValues)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkPipelineLayout layout = %p, VkShaderStageFlags stageFlags = %d, uint32_t offset = %d, uint32_t size = %d, const void* pValues = %p)",
	      commandBuffer, static_cast<void *>(layout), stageFlags, offset, size, pValues);

	vk::Cast(commandBuffer)->pushConstants(vk::Cast(layout), stageFlags, offset, size, pValues);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo *pRenderPassBegin, VkSubpassContents contents)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, const VkRenderPassBeginInfo* pRenderPassBegin = %p, VkSubpassContents contents = %d)",
	      commandBuffer, pRenderPassBegin, contents);

	const VkBaseInStructure *renderPassBeginInfo = reinterpret_cast<const VkBaseInStructure *>(pRenderPassBegin->pNext);
	while(renderPassBeginInfo)
	{
		switch(renderPassBeginInfo->sType)
		{
			case VK_STRUCTURE_TYPE_DEVICE_GROUP_RENDER_PASS_BEGIN_INFO:
				// This extension controls which render area is used on which physical device,
				// in order to distribute rendering between multiple physical devices.
				// SwiftShader only has a single physical device, so this extension does nothing in this case.
				break;
			default:
				LOG_TRAP("pRenderPassBegin->pNext sType = %s", vk::Stringify(renderPassBeginInfo->sType).c_str());
				break;
		}

		renderPassBeginInfo = renderPassBeginInfo->pNext;
	}

	vk::Cast(commandBuffer)->beginRenderPass(vk::Cast(pRenderPassBegin->renderPass), vk::Cast(pRenderPassBegin->framebuffer), pRenderPassBegin->renderArea, pRenderPassBegin->clearValueCount, pRenderPassBegin->pClearValues, contents);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBeginRenderPass2KHR(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo *pRenderPassBegin, const VkSubpassBeginInfoKHR *pSubpassBeginInfo)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, const VkRenderPassBeginInfo* pRenderPassBegin = %p, const VkSubpassBeginInfoKHR* pSubpassBeginInfo = %p)",
	      commandBuffer, pRenderPassBegin, pSubpassBeginInfo);

	vk::Cast(commandBuffer)->beginRenderPass(vk::Cast(pRenderPassBegin->renderPass), vk::Cast(pRenderPassBegin->framebuffer), pRenderPassBegin->renderArea, pRenderPassBegin->clearValueCount, pRenderPassBegin->pClearValues, pSubpassBeginInfo->contents);
}

VKAPI_ATTR void VKAPI_CALL vkCmdNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents contents)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, VkSubpassContents contents = %d)",
	      commandBuffer, contents);

	vk::Cast(commandBuffer)->nextSubpass(contents);
}

VKAPI_ATTR void VKAPI_CALL vkCmdNextSubpass2KHR(VkCommandBuffer commandBuffer, const VkSubpassBeginInfoKHR *pSubpassBeginInfo, const VkSubpassEndInfoKHR *pSubpassEndInfo)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, const VkSubpassBeginInfoKHR* pSubpassBeginInfo = %p, const VkSubpassEndInfoKHR* pSubpassEndInfo = %p)",
	      commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);

	vk::Cast(commandBuffer)->nextSubpass(pSubpassBeginInfo->contents);
}

VKAPI_ATTR void VKAPI_CALL vkCmdEndRenderPass(VkCommandBuffer commandBuffer)
{
	TRACE("(VkCommandBuffer commandBuffer = %p)", commandBuffer);

	vk::Cast(commandBuffer)->endRenderPass();
}

VKAPI_ATTR void VKAPI_CALL vkCmdEndRenderPass2KHR(VkCommandBuffer commandBuffer, const VkSubpassEndInfoKHR *pSubpassEndInfo)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, const VkSubpassEndInfoKHR* pSubpassEndInfo = %p)", commandBuffer, pSubpassEndInfo);

	vk::Cast(commandBuffer)->endRenderPass();
}

VKAPI_ATTR void VKAPI_CALL vkCmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount, const VkCommandBuffer *pCommandBuffers)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, uint32_t commandBufferCount = %d, const VkCommandBuffer* pCommandBuffers = %p)",
	      commandBuffer, commandBufferCount, pCommandBuffers);

	vk::Cast(commandBuffer)->executeCommands(commandBufferCount, pCommandBuffers);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceVersion(uint32_t *pApiVersion)
{
	TRACE("(uint32_t* pApiVersion = %p)", pApiVersion);
	*pApiVersion = vk::API_VERSION;
	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkBindBufferMemory2(VkDevice device, uint32_t bindInfoCount, const VkBindBufferMemoryInfo *pBindInfos)
{
	TRACE("(VkDevice device = %p, uint32_t bindInfoCount = %d, const VkBindBufferMemoryInfo* pBindInfos = %p)",
	      device, bindInfoCount, pBindInfos);

	for(uint32_t i = 0; i < bindInfoCount; i++)
	{
		auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pBindInfos[i].pNext);
		while(extInfo)
		{
			LOG_TRAP("pBindInfos[%d].pNext sType = %s", i, vk::Stringify(extInfo->sType).c_str());
			extInfo = extInfo->pNext;
		}

		if(!vk::Cast(pBindInfos[i].buffer)->canBindToMemory(vk::Cast(pBindInfos[i].memory)))
		{
			UNSUPPORTED("vkBindBufferMemory2 with invalid external memory");
			return VK_ERROR_INVALID_EXTERNAL_HANDLE;
		}
	}

	for(uint32_t i = 0; i < bindInfoCount; i++)
	{
		vk::Cast(pBindInfos[i].buffer)->bind(vk::Cast(pBindInfos[i].memory), pBindInfos[i].memoryOffset);
	}

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkBindImageMemory2(VkDevice device, uint32_t bindInfoCount, const VkBindImageMemoryInfo *pBindInfos)
{
	TRACE("(VkDevice device = %p, uint32_t bindInfoCount = %d, const VkBindImageMemoryInfo* pBindInfos = %p)",
	      device, bindInfoCount, pBindInfos);

	for(uint32_t i = 0; i < bindInfoCount; i++)
	{
		if(!vk::Cast(pBindInfos[i].image)->canBindToMemory(vk::Cast(pBindInfos[i].memory)))
		{
			UNSUPPORTED("vkBindImageMemory2 with invalid external memory");
			return VK_ERROR_OUT_OF_DEVICE_MEMORY;
		}
	}

	for(uint32_t i = 0; i < bindInfoCount; i++)
	{
		vk::DeviceMemory *memory = vk::Cast(pBindInfos[i].memory);
		VkDeviceSize offset = pBindInfos[i].memoryOffset;

		auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pBindInfos[i].pNext);
		while(extInfo)
		{
			switch(extInfo->sType)
			{
				case VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_DEVICE_GROUP_INFO:
					/* Do nothing */
					break;

#ifndef __ANDROID__
				case VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_SWAPCHAIN_INFO_KHR:
				{
					auto swapchainInfo = reinterpret_cast<VkBindImageMemorySwapchainInfoKHR const *>(extInfo);
					memory = vk::Cast(swapchainInfo->swapchain)->getImage(swapchainInfo->imageIndex).getImageMemory();
					offset = 0;
				}
				break;
#endif

				default:
					LOG_TRAP("pBindInfos[%d].pNext sType = %s", i, vk::Stringify(extInfo->sType).c_str());
					break;
			}
			extInfo = extInfo->pNext;
		}

		vk::Cast(pBindInfos[i].image)->bind(memory, offset);
	}

	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetDeviceGroupPeerMemoryFeatures(VkDevice device, uint32_t heapIndex, uint32_t localDeviceIndex, uint32_t remoteDeviceIndex, VkPeerMemoryFeatureFlags *pPeerMemoryFeatures)
{
	TRACE("(VkDevice device = %p, uint32_t heapIndex = %d, uint32_t localDeviceIndex = %d, uint32_t remoteDeviceIndex = %d, VkPeerMemoryFeatureFlags* pPeerMemoryFeatures = %p)",
	      device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);

	ASSERT(localDeviceIndex != remoteDeviceIndex);                 // "localDeviceIndex must not equal remoteDeviceIndex"
	UNSUPPORTED("remoteDeviceIndex: %d", int(remoteDeviceIndex));  // Only one physical device is supported, and since the device indexes can't be equal, this should never be called.
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetDeviceMask(VkCommandBuffer commandBuffer, uint32_t deviceMask)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, uint32_t deviceMask = %d", commandBuffer, deviceMask);

	vk::Cast(commandBuffer)->setDeviceMask(deviceMask);
}

VKAPI_ATTR void VKAPI_CALL vkCmdDispatchBase(VkCommandBuffer commandBuffer, uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, baseGroupX = %u, baseGroupY = %u, baseGroupZ = %u, groupCountX = %u, groupCountY = %u, groupCountZ = %u)",
	      commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);

	vk::Cast(commandBuffer)->dispatchBase(baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDeviceGroups(VkInstance instance, uint32_t *pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties *pPhysicalDeviceGroupProperties)
{
	TRACE("VkInstance instance = %p, uint32_t* pPhysicalDeviceGroupCount = %p, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties = %p",
	      instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);

	return vk::Cast(instance)->getPhysicalDeviceGroups(pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
}

VKAPI_ATTR void VKAPI_CALL vkGetImageMemoryRequirements2(VkDevice device, const VkImageMemoryRequirementsInfo2 *pInfo, VkMemoryRequirements2 *pMemoryRequirements)
{
	TRACE("(VkDevice device = %p, const VkImageMemoryRequirementsInfo2* pInfo = %p, VkMemoryRequirements2* pMemoryRequirements = %p)",
	      device, pInfo, pMemoryRequirements);

	auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pInfo->pNext);
	while(extInfo)
	{
		LOG_TRAP("pInfo->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
		extInfo = extInfo->pNext;
	}

	VkBaseOutStructure *extensionRequirements = reinterpret_cast<VkBaseOutStructure *>(pMemoryRequirements->pNext);
	while(extensionRequirements)
	{
		switch(extensionRequirements->sType)
		{
			case VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS:
			{
				auto requirements = reinterpret_cast<VkMemoryDedicatedRequirements *>(extensionRequirements);
				vk::Cast(device)->getRequirements(requirements);
			}
			break;
			default:
				LOG_TRAP("pMemoryRequirements->pNext sType = %s", vk::Stringify(extensionRequirements->sType).c_str());
				break;
		}

		extensionRequirements = extensionRequirements->pNext;
	}

	vkGetImageMemoryRequirements(device, pInfo->image, &(pMemoryRequirements->memoryRequirements));
}

VKAPI_ATTR void VKAPI_CALL vkGetBufferMemoryRequirements2(VkDevice device, const VkBufferMemoryRequirementsInfo2 *pInfo, VkMemoryRequirements2 *pMemoryRequirements)
{
	TRACE("(VkDevice device = %p, const VkBufferMemoryRequirementsInfo2* pInfo = %p, VkMemoryRequirements2* pMemoryRequirements = %p)",
	      device, pInfo, pMemoryRequirements);

	auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pInfo->pNext);
	while(extInfo)
	{
		LOG_TRAP("pInfo->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
		extInfo = extInfo->pNext;
	}

	VkBaseOutStructure *extensionRequirements = reinterpret_cast<VkBaseOutStructure *>(pMemoryRequirements->pNext);
	while(extensionRequirements)
	{
		switch(extensionRequirements->sType)
		{
			case VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS:
			{
				auto requirements = reinterpret_cast<VkMemoryDedicatedRequirements *>(extensionRequirements);
				vk::Cast(device)->getRequirements(requirements);
			}
			break;
			default:
				LOG_TRAP("pMemoryRequirements->pNext sType = %s", vk::Stringify(extensionRequirements->sType).c_str());
				break;
		}

		extensionRequirements = extensionRequirements->pNext;
	}

	vkGetBufferMemoryRequirements(device, pInfo->buffer, &(pMemoryRequirements->memoryRequirements));
}

VKAPI_ATTR void VKAPI_CALL vkGetImageSparseMemoryRequirements2(VkDevice device, const VkImageSparseMemoryRequirementsInfo2 *pInfo, uint32_t *pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements2 *pSparseMemoryRequirements)
{
	TRACE("(VkDevice device = %p, const VkImageSparseMemoryRequirementsInfo2* pInfo = %p, uint32_t* pSparseMemoryRequirementCount = %p, VkSparseImageMemoryRequirements2* pSparseMemoryRequirements = %p)",
	      device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);

	auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pInfo->pNext);
	while(extInfo)
	{
		LOG_TRAP("pInfo->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
		extInfo = extInfo->pNext;
	}

	auto extensionRequirements = reinterpret_cast<VkBaseInStructure const *>(pSparseMemoryRequirements->pNext);
	while(extensionRequirements)
	{
		LOG_TRAP("pSparseMemoryRequirements->pNext sType = %s", vk::Stringify(extensionRequirements->sType).c_str());
		extensionRequirements = extensionRequirements->pNext;
	}

	// The 'sparseBinding' feature is not supported, so images can not be created with the VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT flag.
	// "If the image was not created with VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT then pSparseMemoryRequirementCount will be set to zero and pSparseMemoryRequirements will not be written to."
	*pSparseMemoryRequirementCount = 0;
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2 *pFeatures)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, VkPhysicalDeviceFeatures2* pFeatures = %p)", physicalDevice, pFeatures);

	VkBaseOutStructure *extensionFeatures = reinterpret_cast<VkBaseOutStructure *>(pFeatures->pNext);
	while(extensionFeatures)
	{
		switch((long)(extensionFeatures->sType))
		{
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES:
			{
				auto features = reinterpret_cast<VkPhysicalDeviceSamplerYcbcrConversionFeatures *>(extensionFeatures);
				vk::Cast(physicalDevice)->getFeatures(features);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES:
			{
				auto features = reinterpret_cast<VkPhysicalDevice16BitStorageFeatures *>(extensionFeatures);
				vk::Cast(physicalDevice)->getFeatures(features);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES:
			{
				auto features = reinterpret_cast<VkPhysicalDeviceVariablePointerFeatures *>(extensionFeatures);
				vk::Cast(physicalDevice)->getFeatures(features);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR:
			{
				auto features = reinterpret_cast<VkPhysicalDevice8BitStorageFeaturesKHR *>(extensionFeatures);
				vk::Cast(physicalDevice)->getFeatures(features);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES:
			{
				auto features = reinterpret_cast<VkPhysicalDeviceMultiviewFeatures *>(extensionFeatures);
				vk::Cast(physicalDevice)->getFeatures(features);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES:
			{
				auto features = reinterpret_cast<VkPhysicalDeviceProtectedMemoryFeatures *>(extensionFeatures);
				vk::Cast(physicalDevice)->getFeatures(features);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES:
			{
				auto features = reinterpret_cast<VkPhysicalDeviceShaderDrawParameterFeatures *>(extensionFeatures);
				vk::Cast(physicalDevice)->getFeatures(features);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES_KHR:
			{
				auto features = reinterpret_cast<VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *>(extensionFeatures);
				vk::Cast(physicalDevice)->getFeatures(features);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT:
			{
				auto features = reinterpret_cast<VkPhysicalDeviceLineRasterizationFeaturesEXT *>(extensionFeatures);
				vk::Cast(physicalDevice)->getFeatures(features);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT:
			{
				auto features = reinterpret_cast<VkPhysicalDeviceProvokingVertexFeaturesEXT *>(extensionFeatures);
				vk::Cast(physicalDevice)->getFeatures(features);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT:
				ASSERT(!HasExtensionProperty(VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME, deviceExtensionProperties,
				                             sizeof(deviceExtensionProperties) / sizeof(deviceExtensionProperties[0])));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT:
				ASSERT(!HasExtensionProperty(VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME, deviceExtensionProperties,
				                             sizeof(deviceExtensionProperties) / sizeof(deviceExtensionProperties[0])));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR:
				ASSERT(!HasExtensionProperty(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME, deviceExtensionProperties,
				                             sizeof(deviceExtensionProperties) / sizeof(deviceExtensionProperties[0])));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR:
				ASSERT(!HasExtensionProperty(VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME, deviceExtensionProperties,
				                             sizeof(deviceExtensionProperties) / sizeof(deviceExtensionProperties[0])));
				break;
			default:
				LOG_TRAP("pFeatures->pNext sType = %s", vk::Stringify(extensionFeatures->sType).c_str());
				break;
		}

		extensionFeatures = extensionFeatures->pNext;
	}

	vkGetPhysicalDeviceFeatures(physicalDevice, &(pFeatures->features));
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties2 *pProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, VkPhysicalDeviceProperties2* pProperties = %p)", physicalDevice, pProperties);

	VkBaseOutStructure *extensionProperties = reinterpret_cast<VkBaseOutStructure *>(pProperties->pNext);
	while(extensionProperties)
	{
		// Casting to a long since some structures, such as
		// VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENTATION_PROPERTIES_ANDROID and
		// VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_PROPERTIES_EXT
		// are not enumerated in the official Vulkan header
		switch((long)(extensionProperties->sType))
		{
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES:
			{
				auto properties = reinterpret_cast<VkPhysicalDeviceIDProperties *>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(properties);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES:
			{
				auto properties = reinterpret_cast<VkPhysicalDeviceMaintenance3Properties *>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(properties);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES:
			{
				auto properties = reinterpret_cast<VkPhysicalDeviceMultiviewProperties *>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(properties);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES:
			{
				auto properties = reinterpret_cast<VkPhysicalDevicePointClippingProperties *>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(properties);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_PROPERTIES:
			{
				auto properties = reinterpret_cast<VkPhysicalDeviceProtectedMemoryProperties *>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(properties);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES:
			{
				auto properties = reinterpret_cast<VkPhysicalDeviceSubgroupProperties *>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(properties);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT:
				// Explicitly ignored, since VK_EXT_sample_locations is not supported
				ASSERT(!HasExtensionProperty(VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME, deviceExtensionProperties,
				                             sizeof(deviceExtensionProperties) / sizeof(deviceExtensionProperties[0])));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT:
			{
				auto properties = reinterpret_cast<VkPhysicalDeviceExternalMemoryHostPropertiesEXT *>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(properties);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR:
			{
				auto properties = reinterpret_cast<VkPhysicalDeviceDriverPropertiesKHR *>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(properties);
			}
			break;
#ifdef __ANDROID__
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENTATION_PROPERTIES_ANDROID:
			{
				auto properties = reinterpret_cast<VkPhysicalDevicePresentationPropertiesANDROID *>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(properties);
			}
			break;
#endif
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES_EXT:
			{
				auto properties = reinterpret_cast<VkPhysicalDeviceLineRasterizationPropertiesEXT *>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(properties);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_PROPERTIES_EXT:
			{
				auto properties = reinterpret_cast<VkPhysicalDeviceProvokingVertexPropertiesEXT *>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(properties);
			}
			break;
			default:
				// "the [driver] must skip over, without processing (other than reading the sType and pNext members) any structures in the chain with sType values not defined by [supported extenions]"
				LOG_TRAP("pProperties->pNext sType = %s", vk::Stringify(extensionProperties->sType).c_str());
				break;
		}

		extensionProperties = extensionProperties->pNext;
	}

	vkGetPhysicalDeviceProperties(physicalDevice, &(pProperties->properties));
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFormatProperties2(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties2 *pFormatProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, VkFormat format = %d, VkFormatProperties2* pFormatProperties = %p)",
	      physicalDevice, format, pFormatProperties);

	auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pFormatProperties->pNext);
	while(extInfo)
	{
		LOG_TRAP("pFormatProperties->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
		extInfo = extInfo->pNext;
	}

	vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &(pFormatProperties->formatProperties));
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceImageFormatProperties2(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2 *pImageFormatInfo, VkImageFormatProperties2 *pImageFormatProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo = %p, VkImageFormatProperties2* pImageFormatProperties = %p)",
	      physicalDevice, pImageFormatInfo, pImageFormatProperties);

	const VkBaseInStructure *extensionFormatInfo = reinterpret_cast<const VkBaseInStructure *>(pImageFormatInfo->pNext);

	const VkExternalMemoryHandleTypeFlagBits *handleType = nullptr;
	while(extensionFormatInfo)
	{
		switch(extensionFormatInfo->sType)
		{
			case VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO_KHR:
			{
				// Explicitly ignored, since VK_KHR_image_format_list is not supported
				ASSERT(!HasExtensionProperty(VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME, deviceExtensionProperties,
				                             sizeof(deviceExtensionProperties) / sizeof(deviceExtensionProperties[0])));
			}
			break;
			case VK_STRUCTURE_TYPE_IMAGE_STENCIL_USAGE_CREATE_INFO_EXT:
			{
				// Explicitly ignored, since VK_EXT_separate_stencil_usage is not supported
				ASSERT(!HasExtensionProperty(VK_EXT_SEPARATE_STENCIL_USAGE_EXTENSION_NAME, deviceExtensionProperties,
				                             sizeof(deviceExtensionProperties) / sizeof(deviceExtensionProperties[0])));
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_IMAGE_FORMAT_INFO:
			{
				const VkPhysicalDeviceExternalImageFormatInfo *imageFormatInfo = reinterpret_cast<const VkPhysicalDeviceExternalImageFormatInfo *>(extensionFormatInfo);
				handleType = &(imageFormatInfo->handleType);
			}
			break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_DRM_FORMAT_MODIFIER_INFO_EXT:
			{
				// Explicitly ignored, since VK_EXT_image_drm_format_modifier is not supported
				ASSERT(!HasExtensionProperty(VK_EXT_IMAGE_DRM_FORMAT_MODIFIER_EXTENSION_NAME, deviceExtensionProperties,
				                             sizeof(deviceExtensionProperties) / sizeof(deviceExtensionProperties[0])));
			}
			break;
			default:
				LOG_TRAP("pImageFormatInfo->pNext sType = %s", vk::Stringify(extensionFormatInfo->sType).c_str());
				break;
		}

		extensionFormatInfo = extensionFormatInfo->pNext;
	}

	VkBaseOutStructure *extensionProperties = reinterpret_cast<VkBaseOutStructure *>(pImageFormatProperties->pNext);

	while(extensionProperties)
	{
		switch(extensionProperties->sType)
		{
			case VK_STRUCTURE_TYPE_EXTERNAL_IMAGE_FORMAT_PROPERTIES:
			{
				auto properties = reinterpret_cast<VkExternalImageFormatProperties *>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(handleType, properties);
			}
			break;
			case VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_IMAGE_FORMAT_PROPERTIES:
			{
				auto properties = reinterpret_cast<VkSamplerYcbcrConversionImageFormatProperties *>(extensionProperties);
				vk::Cast(physicalDevice)->getProperties(properties);
			}
			break;
			case VK_STRUCTURE_TYPE_TEXTURE_LOD_GATHER_FORMAT_PROPERTIES_AMD:
			{
				// Explicitly ignored, since VK_AMD_texture_gather_bias_lod is not supported
				ASSERT(!HasExtensionProperty(VK_AMD_TEXTURE_GATHER_BIAS_LOD_EXTENSION_NAME, deviceExtensionProperties,
				                             sizeof(deviceExtensionProperties) / sizeof(deviceExtensionProperties[0])));
			}
			break;
			default:
				LOG_TRAP("pImageFormatProperties->pNext sType = %s", vk::Stringify(extensionProperties->sType).c_str());
				break;
		}

		extensionProperties = extensionProperties->pNext;
	}

	return vkGetPhysicalDeviceImageFormatProperties(physicalDevice,
	                                                pImageFormatInfo->format,
	                                                pImageFormatInfo->type,
	                                                pImageFormatInfo->tiling,
	                                                pImageFormatInfo->usage,
	                                                pImageFormatInfo->flags,
	                                                &(pImageFormatProperties->imageFormatProperties));
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties2(VkPhysicalDevice physicalDevice, uint32_t *pQueueFamilyPropertyCount, VkQueueFamilyProperties2 *pQueueFamilyProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, uint32_t* pQueueFamilyPropertyCount = %p, VkQueueFamilyProperties2* pQueueFamilyProperties = %p)",
	      physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);

	if(pQueueFamilyProperties)
	{
		auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pQueueFamilyProperties->pNext);
		while(extInfo)
		{
			LOG_TRAP("pQueueFamilyProperties->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
			extInfo = extInfo->pNext;
		}
	}

	if(!pQueueFamilyProperties)
	{
		*pQueueFamilyPropertyCount = vk::Cast(physicalDevice)->getQueueFamilyPropertyCount();
	}
	else
	{
		vk::Cast(physicalDevice)->getQueueFamilyProperties(*pQueueFamilyPropertyCount, pQueueFamilyProperties);
	}
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceMemoryProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2 *pMemoryProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, VkPhysicalDeviceMemoryProperties2* pMemoryProperties = %p)", physicalDevice, pMemoryProperties);

	auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pMemoryProperties->pNext);
	while(extInfo)
	{
		LOG_TRAP("pMemoryProperties->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
		extInfo = extInfo->pNext;
	}

	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &(pMemoryProperties->memoryProperties));
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceSparseImageFormatProperties2(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2 *pFormatInfo, uint32_t *pPropertyCount, VkSparseImageFormatProperties2 *pProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo = %p, uint32_t* pPropertyCount = %p, VkSparseImageFormatProperties2* pProperties = %p)",
	      physicalDevice, pFormatInfo, pPropertyCount, pProperties);

	if(pProperties)
	{
		auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pProperties->pNext);
		while(extInfo)
		{
			LOG_TRAP("pProperties->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
			extInfo = extInfo->pNext;
		}
	}

	// We do not support sparse images.
	*pPropertyCount = 0;
}

VKAPI_ATTR void VKAPI_CALL vkTrimCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolTrimFlags flags)
{
	TRACE("(VkDevice device = %p, VkCommandPool commandPool = %p, VkCommandPoolTrimFlags flags = %d)",
	      device, static_cast<void *>(commandPool), flags);

	if(flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("flags %d", int(flags));
	}

	vk::Cast(commandPool)->trim(flags);
}

VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue2(VkDevice device, const VkDeviceQueueInfo2 *pQueueInfo, VkQueue *pQueue)
{
	TRACE("(VkDevice device = %p, const VkDeviceQueueInfo2* pQueueInfo = %p, VkQueue* pQueue = %p)",
	      device, pQueueInfo, pQueue);

	auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pQueueInfo->pNext);
	while(extInfo)
	{
		LOG_TRAP("pQueueInfo->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
		extInfo = extInfo->pNext;
	}

	if(pQueueInfo->flags != 0)
	{
		// The only flag that can be set here is VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT
		// According to the Vulkan 1.2.132 spec, 4.3.1. Queue Family Properties:
		// "VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT specifies that the device queue is a
		//  protected-capable queue. If the protected memory feature is not enabled,
		//  the VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT bit of flags must not be set."
		UNSUPPORTED("VkPhysicalDeviceVulkan11Features::protectedMemory");
	}

	vkGetDeviceQueue(device, pQueueInfo->queueFamilyIndex, pQueueInfo->queueIndex, pQueue);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateSamplerYcbcrConversion(VkDevice device, const VkSamplerYcbcrConversionCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSamplerYcbcrConversion *pYcbcrConversion)
{
	TRACE("(VkDevice device = %p, const VkSamplerYcbcrConversionCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkSamplerYcbcrConversion* pYcbcrConversion = %p)",
	      device, pCreateInfo, pAllocator, pYcbcrConversion);

	auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pCreateInfo->pNext);
	while(extInfo)
	{
		LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
		extInfo = extInfo->pNext;
	}

	return vk::SamplerYcbcrConversion::Create(pAllocator, pCreateInfo, pYcbcrConversion);
}

VKAPI_ATTR void VKAPI_CALL vkDestroySamplerYcbcrConversion(VkDevice device, VkSamplerYcbcrConversion ycbcrConversion, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkSamplerYcbcrConversion ycbcrConversion = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(ycbcrConversion), pAllocator);

	vk::destroy(ycbcrConversion, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorUpdateTemplate(VkDevice device, const VkDescriptorUpdateTemplateCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDescriptorUpdateTemplate *pDescriptorUpdateTemplate)
{
	TRACE("(VkDevice device = %p, const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate = %p)",
	      device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);

	if(pCreateInfo->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	if(pCreateInfo->templateType != VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET)
	{
		UNSUPPORTED("pCreateInfo->templateType %d", int(pCreateInfo->templateType));
	}

	auto extInfo = reinterpret_cast<VkBaseInStructure const *>(pCreateInfo->pNext);
	while(extInfo)
	{
		LOG_TRAP("pCreateInfo->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
		extInfo = extInfo->pNext;
	}

	return vk::DescriptorUpdateTemplate::Create(pAllocator, pCreateInfo, pDescriptorUpdateTemplate);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorUpdateTemplate(VkDevice device, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkDescriptorUpdateTemplate descriptorUpdateTemplate = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(descriptorUpdateTemplate), pAllocator);

	vk::destroy(descriptorUpdateTemplate, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL vkUpdateDescriptorSetWithTemplate(VkDevice device, VkDescriptorSet descriptorSet, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const void *pData)
{
	TRACE("(VkDevice device = %p, VkDescriptorSet descriptorSet = %p, VkDescriptorUpdateTemplate descriptorUpdateTemplate = %p, const void* pData = %p)",
	      device, static_cast<void *>(descriptorSet), static_cast<void *>(descriptorUpdateTemplate), pData);

	vk::Cast(descriptorUpdateTemplate)->updateDescriptorSet(vk::Cast(device), descriptorSet, pData);
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalBufferProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalBufferInfo *pExternalBufferInfo, VkExternalBufferProperties *pExternalBufferProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo = %p, VkExternalBufferProperties* pExternalBufferProperties = %p)",
	      physicalDevice, pExternalBufferInfo, pExternalBufferProperties);

	vk::Cast(physicalDevice)->getProperties(pExternalBufferInfo, pExternalBufferProperties);
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalFenceProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalFenceInfo *pExternalFenceInfo, VkExternalFenceProperties *pExternalFenceProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo = %p, VkExternalFenceProperties* pExternalFenceProperties = %p)",
	      physicalDevice, pExternalFenceInfo, pExternalFenceProperties);

	vk::Cast(physicalDevice)->getProperties(pExternalFenceInfo, pExternalFenceProperties);
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalSemaphoreProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo *pExternalSemaphoreInfo, VkExternalSemaphoreProperties *pExternalSemaphoreProperties)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo = %p, VkExternalSemaphoreProperties* pExternalSemaphoreProperties = %p)",
	      physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);

	vk::Cast(physicalDevice)->getProperties(pExternalSemaphoreInfo, pExternalSemaphoreProperties);
}

VKAPI_ATTR void VKAPI_CALL vkGetDescriptorSetLayoutSupport(VkDevice device, const VkDescriptorSetLayoutCreateInfo *pCreateInfo, VkDescriptorSetLayoutSupport *pSupport)
{
	TRACE("(VkDevice device = %p, const VkDescriptorSetLayoutCreateInfo* pCreateInfo = %p, VkDescriptorSetLayoutSupport* pSupport = %p)",
	      device, pCreateInfo, pSupport);

	vk::Cast(device)->getDescriptorSetLayoutSupport(pCreateInfo, pSupport);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetLineStippleEXT(VkCommandBuffer commandBuffer, uint32_t lineStippleFactor, uint16_t lineStipplePattern)
{
	TRACE("(VkCommandBuffer commandBuffer = %p, uint32_t lineStippleFactor = %u, uint16_t lineStipplePattern = %u",
	      commandBuffer, lineStippleFactor, lineStipplePattern);

	UNSUPPORTED("VkPhysicalDeviceLineRasterizationFeaturesEXT::stippled*Lines");
}

#ifdef VK_USE_PLATFORM_XCB_KHR
VKAPI_ATTR VkResult VKAPI_CALL vkCreateXcbSurfaceKHR(VkInstance instance, const VkXcbSurfaceCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface)
{
	TRACE("(VkInstance instance = %p, VkXcbSurfaceCreateInfoKHR* pCreateInfo = %p, VkAllocationCallbacks* pAllocator = %p, VkSurface* pSurface = %p)",
	      instance, pCreateInfo, pAllocator, pSurface);

	return vk::XcbSurfaceKHR::Create(pAllocator, pCreateInfo, pSurface);
}

VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceXcbPresentationSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, xcb_connection_t *connection, xcb_visualid_t visual_id)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, uint32_t queueFamilyIndex = %d, xcb_connection_t* connection = %p, xcb_visualid_t visual_id = %d)",
	      physicalDevice, int(queueFamilyIndex), connection, int(visual_id));

	return VK_TRUE;
}
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
VKAPI_ATTR VkResult VKAPI_CALL vkCreateXlibSurfaceKHR(VkInstance instance, const VkXlibSurfaceCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface)
{
	TRACE("(VkInstance instance = %p, VkXlibSurfaceCreateInfoKHR* pCreateInfo = %p, VkAllocationCallbacks* pAllocator = %p, VkSurface* pSurface = %p)",
	      instance, pCreateInfo, pAllocator, pSurface);

	return vk::XlibSurfaceKHR::Create(pAllocator, pCreateInfo, pSurface);
}

VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceXlibPresentationSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, Display *dpy, VisualID visualID)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, uint32_t queueFamilyIndex = %d, Display* dpy = %p, VisualID visualID = %lu)",
	      physicalDevice, int(queueFamilyIndex), dpy, visualID);

	return VK_TRUE;
}
#endif

#ifdef VK_USE_PLATFORM_MACOS_MVK
VKAPI_ATTR VkResult VKAPI_CALL vkCreateMacOSSurfaceMVK(VkInstance instance, const VkMacOSSurfaceCreateInfoMVK *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface)
{
	TRACE("(VkInstance instance = %p, VkMacOSSurfaceCreateInfoMVK* pCreateInfo = %p, VkAllocationCallbacks* pAllocator = %p, VkSurface* pSurface = %p)",
	      instance, pCreateInfo, pAllocator, pSurface);

	return vk::MacOSSurfaceMVK::Create(pAllocator, pCreateInfo, pSurface);
}
#endif

#ifdef VK_USE_PLATFORM_METAL_EXT
VKAPI_ATTR VkResult VKAPI_CALL vkCreateMetalSurfaceEXT(VkInstance instance, const VkMetalSurfaceCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface)
{
	TRACE("(VkInstance instance = %p, VkMetalSurfaceCreateInfoEXT* pCreateInfo = %p, VkAllocationCallbacks* pAllocator = %p, VkSurface* pSurface = %p)",
	      instance, pCreateInfo, pAllocator, pSurface);

	return vk::MetalSurfaceEXT::Create(pAllocator, pCreateInfo, pSurface);
}
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL vkCreateWin32SurfaceKHR(VkInstance instance, const VkWin32SurfaceCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface)
{
	TRACE("(VkInstance instance = %p, VkWin32SurfaceCreateInfoKHR* pCreateInfo = %p, VkAllocationCallbacks* pAllocator = %p, VkSurface* pSurface = %p)",
	      instance, pCreateInfo, pAllocator, pSurface);

	return vk::Win32SurfaceKHR::Create(pAllocator, pCreateInfo, pSurface);
}

VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceWin32PresentationSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, uint32_t queueFamilyIndex = %d)",
	      physicalDevice, queueFamilyIndex);
	return VK_TRUE;
}
#endif

#ifndef __ANDROID__
VKAPI_ATTR void VKAPI_CALL vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkInstance instance = %p, VkSurfaceKHR surface = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      instance, static_cast<void *>(surface), pAllocator);

	vk::destroy(surface, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32 *pSupported)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, uint32_t queueFamilyIndex = %d, VkSurface surface = %p, VKBool32* pSupported = %p)",
	      physicalDevice, int(queueFamilyIndex), static_cast<void *>(surface), pSupported);

	*pSupported = VK_TRUE;
	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR *pSurfaceCapabilities)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, VkSurfaceKHR surface = %p, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities = %p)",
	      physicalDevice, static_cast<void *>(surface), pSurfaceCapabilities);

	vk::Cast(surface)->getSurfaceCapabilities(pSurfaceCapabilities);
	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t *pSurfaceFormatCount, VkSurfaceFormatKHR *pSurfaceFormats)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, VkSurfaceKHR surface = %p. uint32_t* pSurfaceFormatCount = %p, VkSurfaceFormatKHR* pSurfaceFormats = %p)",
	      physicalDevice, static_cast<void *>(surface), pSurfaceFormatCount, pSurfaceFormats);

	if(!pSurfaceFormats)
	{
		*pSurfaceFormatCount = vk::Cast(surface)->getSurfaceFormatsCount();
		return VK_SUCCESS;
	}

	return vk::Cast(surface)->getSurfaceFormats(pSurfaceFormatCount, pSurfaceFormats);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t *pPresentModeCount, VkPresentModeKHR *pPresentModes)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, VkSurfaceKHR surface = %p uint32_t* pPresentModeCount = %p, VkPresentModeKHR* pPresentModes = %p)",
	      physicalDevice, static_cast<void *>(surface), pPresentModeCount, pPresentModes);

	if(!pPresentModes)
	{
		*pPresentModeCount = vk::Cast(surface)->getPresentModeCount();
		return VK_SUCCESS;
	}

	return vk::Cast(surface)->getPresentModes(pPresentModeCount, pPresentModes);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSwapchainKHR *pSwapchain)
{
	TRACE("(VkDevice device = %p, const VkSwapchainCreateInfoKHR* pCreateInfo = %p, const VkAllocationCallbacks* pAllocator = %p, VkSwapchainKHR* pSwapchain = %p)",
	      device, pCreateInfo, pAllocator, pSwapchain);

	if(pCreateInfo->oldSwapchain)
	{
		vk::Cast(pCreateInfo->oldSwapchain)->retire();
	}

	if(vk::Cast(pCreateInfo->surface)->hasAssociatedSwapchain())
	{
		return VK_ERROR_NATIVE_WINDOW_IN_USE_KHR;
	}

	VkResult status = vk::SwapchainKHR::Create(pAllocator, pCreateInfo, pSwapchain);

	if(status != VK_SUCCESS)
	{
		return status;
	}

	auto swapchain = vk::Cast(*pSwapchain);
	status = swapchain->createImages(device, pCreateInfo);

	if(status != VK_SUCCESS)
	{
		vk::destroy(*pSwapchain, pAllocator);
		return status;
	}

	vk::Cast(pCreateInfo->surface)->associateSwapchain(swapchain);

	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks *pAllocator)
{
	TRACE("(VkDevice device = %p, VkSwapchainKHR swapchain = %p, const VkAllocationCallbacks* pAllocator = %p)",
	      device, static_cast<void *>(swapchain), pAllocator);

	vk::destroy(swapchain, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain, uint32_t *pSwapchainImageCount, VkImage *pSwapchainImages)
{
	TRACE("(VkDevice device = %p, VkSwapchainKHR swapchain = %p, uint32_t* pSwapchainImageCount = %p, VkImage* pSwapchainImages = %p)",
	      device, static_cast<void *>(swapchain), pSwapchainImageCount, pSwapchainImages);

	if(!pSwapchainImages)
	{
		*pSwapchainImageCount = vk::Cast(swapchain)->getImageCount();
		return VK_SUCCESS;
	}

	return vk::Cast(swapchain)->getImages(pSwapchainImageCount, pSwapchainImages);
}

VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t *pImageIndex)
{
	TRACE("(VkDevice device = %p, VkSwapchainKHR swapchain = %p, uint64_t timeout = %d, VkSemaphore semaphore = %p, VkFence fence = %p, uint32_t* pImageIndex = %p)",
	      device, static_cast<void *>(swapchain), int(timeout), static_cast<void *>(semaphore), static_cast<void *>(fence), pImageIndex);

	return vk::Cast(swapchain)->getNextImage(timeout, vk::Cast(semaphore), vk::Cast(fence), pImageIndex);
}

VKAPI_ATTR VkResult VKAPI_CALL vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR *pPresentInfo)
{
	TRACE("(VkQueue queue = %p, const VkPresentInfoKHR* pPresentInfo = %p)",
	      queue, pPresentInfo);

	return vk::Cast(queue)->present(pPresentInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImage2KHR(VkDevice device, const VkAcquireNextImageInfoKHR *pAcquireInfo, uint32_t *pImageIndex)
{
	TRACE("(VkDevice device = %p, const VkAcquireNextImageInfoKHR *pAcquireInfo = %p, uint32_t *pImageIndex = %p",
	      device, pAcquireInfo, pImageIndex);

	return vk::Cast(pAcquireInfo->swapchain)->getNextImage(pAcquireInfo->timeout, vk::Cast(pAcquireInfo->semaphore), vk::Cast(pAcquireInfo->fence), pImageIndex);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetDeviceGroupPresentCapabilitiesKHR(VkDevice device, VkDeviceGroupPresentCapabilitiesKHR *pDeviceGroupPresentCapabilities)
{
	TRACE("(VkDevice device = %p, VkDeviceGroupPresentCapabilitiesKHR* pDeviceGroupPresentCapabilities = %p)",
	      device, pDeviceGroupPresentCapabilities);

	for(int i = 0; i < VK_MAX_DEVICE_GROUP_SIZE; i++)
	{
		// The only real physical device in the presentation group is device 0,
		// and it can present to itself.
		pDeviceGroupPresentCapabilities->presentMask[i] = (i == 0) ? 1 : 0;
	}

	pDeviceGroupPresentCapabilities->modes = VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR;

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetDeviceGroupSurfacePresentModesKHR(VkDevice device, VkSurfaceKHR surface, VkDeviceGroupPresentModeFlagsKHR *pModes)
{
	TRACE("(VkDevice device = %p, VkSurfaceKHR surface = %p, VkDeviceGroupPresentModeFlagsKHR *pModes = %p)",
	      device, static_cast<void *>(surface), pModes);

	*pModes = VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR;
	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDevicePresentRectanglesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t *pRectCount, VkRect2D *pRects)
{
	TRACE("(VkPhysicalDevice physicalDevice = %p, VkSurfaceKHR surface = %p, uint32_t* pRectCount = %p, VkRect2D* pRects = %p)",
	      physicalDevice, static_cast<void *>(surface), pRectCount, pRects);

	return vk::Cast(surface)->getPresentRectangles(pRectCount, pRects);
}

#endif  // ! __ANDROID__

#ifdef __ANDROID__

VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainGrallocUsage2ANDROID(VkDevice device, VkFormat format, VkImageUsageFlags imageUsage, VkSwapchainImageUsageFlagsANDROID swapchainUsage, uint64_t *grallocConsumerUsage, uint64_t *grallocProducerUsage)
{
	TRACE("(VkDevice device = %p, VkFormat format = %d, VkImageUsageFlags imageUsage = %d, VkSwapchainImageUsageFlagsANDROID swapchainUsage = %d, uint64_t* grallocConsumerUsage = %p, uin64_t* grallocProducerUsage = %p)",
	      device, format, imageUsage, swapchainUsage, grallocConsumerUsage, grallocProducerUsage);

	*grallocConsumerUsage = 0;
	*grallocProducerUsage = GRALLOC1_PRODUCER_USAGE_CPU_WRITE_OFTEN;

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainGrallocUsageANDROID(VkDevice device, VkFormat format, VkImageUsageFlags imageUsage, int *grallocUsage)
{
	TRACE("(VkDevice device = %p, VkFormat format = %d, VkImageUsageFlags imageUsage = %d, int* grallocUsage = %p)",
	      device, format, imageUsage, grallocUsage);

	*grallocUsage = GRALLOC_USAGE_SW_WRITE_OFTEN;

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkAcquireImageANDROID(VkDevice device, VkImage image, int nativeFenceFd, VkSemaphore semaphore, VkFence fence)
{
	TRACE("(VkDevice device = %p, VkImage image = %p, int nativeFenceFd = %d, VkSemaphore semaphore = %p, VkFence fence = %p)",
	      device, static_cast<void *>(image), nativeFenceFd, static_cast<void *>(semaphore), static_cast<void *>(fence));

	if(nativeFenceFd >= 0)
	{
		sync_wait(nativeFenceFd, -1);
		close(nativeFenceFd);
	}

	if(fence != VK_NULL_HANDLE)
	{
		vk::Cast(fence)->complete();
	}

	if(semaphore != VK_NULL_HANDLE)
	{
		vk::Cast(semaphore)->signal();
	}

	return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkQueueSignalReleaseImageANDROID(VkQueue queue, uint32_t waitSemaphoreCount, const VkSemaphore *pWaitSemaphores, VkImage image, int *pNativeFenceFd)
{
	TRACE("(VkQueue queue = %p, uint32_t waitSemaphoreCount = %d, const VkSemaphore* pWaitSemaphores = %p, VkImage image = %p, int* pNativeFenceFd = %p)",
	      queue, waitSemaphoreCount, pWaitSemaphores, static_cast<void *>(image), pNativeFenceFd);

	// This is a hack to deal with screen tearing for now.
	// Need to correctly implement threading using VkSemaphore
	// to get rid of it. b/132458423
	vkQueueWaitIdle(queue);

	*pNativeFenceFd = -1;

	return vk::Cast(image)->prepareForExternalUseANDROID();
}
#endif  // __ANDROID__
}
