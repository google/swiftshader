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

#include "VkPhysicalDevice.hpp"
#include "VkConfig.h"
#include <memory.h>

namespace vk
{

PhysicalDevice::PhysicalDevice(const void*, void* mem)
{
}

const VkPhysicalDeviceFeatures& PhysicalDevice::getFeatures() const
{
	static const VkPhysicalDeviceFeatures features
	{
		true, // robustBufferAccess
		false, // fullDrawIndexUint32
		false, // imageCubeArray
		false, // independentBlend
		false, // geometryShader
		false, // tessellationShader
		false, // sampleRateShading
		false, // dualSrcBlend
		false, // logicOp
		false, // multiDrawIndirect
		false, // drawIndirectFirstInstance
		false, // depthClamp
		false, // depthBiasClamp
		false, // fillModeNonSolid
		false, // depthBounds
		false, // wideLines
		false, // largePoints
		false, // alphaToOne
		false, // multiViewport
		false, // samplerAnisotropy
		false, // textureCompressionETC2
		false, // textureCompressionASTC_LDR
		false, // textureCompressionBC
		false, // occlusionQueryPrecise
		false, // pipelineStatisticsQuery
		false, // vertexPipelineStoresAndAtomics
		false, // fragmentStoresAndAtomics
		false, // shaderTessellationAndGeometryPointSize
		false, // shaderImageGatherExtended
		false, // shaderStorageImageExtendedFormats
		false, // shaderStorageImageMultisample
		false, // shaderStorageImageReadWithoutFormat
		false, // shaderStorageImageWriteWithoutFormat
		false, // shaderUniformBufferArrayDynamicIndexing
		false, // shaderSampledImageArrayDynamicIndexing
		false, // shaderStorageBufferArrayDynamicIndexing
		false, // shaderStorageImageArrayDynamicIndexing
		false, // shaderClipDistance
		false, // shaderCullDistance
		false, // shaderFloat64
		false, // shaderInt64
		false, // shaderInt16
		false, // shaderResourceResidency
		false, // shaderResourceMinLod
		false, // sparseBinding
		false, // sparseResidencyBuffer
		false, // sparseResidencyImage2D
		false, // sparseResidencyImage3D
		false, // sparseResidency2Samples
		false, // sparseResidency4Samples
		false, // sparseResidency8Samples
		false, // sparseResidency16Samples
		false, // sparseResidencyAliased
		false, // variableMultisampleRate
		false, // inheritedQueries
	};

	return features;
}

void PhysicalDevice::getFeatures(VkPhysicalDeviceSamplerYcbcrConversionFeatures* features) const
{
	features->samplerYcbcrConversion = VK_FALSE;
}

void PhysicalDevice::getFeatures(VkPhysicalDevice16BitStorageFeatures* features) const
{
	features->storageBuffer16BitAccess = VK_FALSE;
	features->storageInputOutput16 = VK_FALSE;
	features->storagePushConstant16 = VK_FALSE;
	features->uniformAndStorageBuffer16BitAccess = VK_FALSE;
}

void PhysicalDevice::getFeatures(VkPhysicalDeviceVariablePointerFeatures* features) const
{
	features->variablePointersStorageBuffer = VK_FALSE;
	features->variablePointers = VK_FALSE;
}

void PhysicalDevice::getFeatures(VkPhysicalDevice8BitStorageFeaturesKHR* features) const
{
	features->storageBuffer8BitAccess = VK_FALSE;
	features->uniformAndStorageBuffer8BitAccess = VK_FALSE;
	features->storagePushConstant8 = VK_FALSE;
}

void PhysicalDevice::getFeatures(VkPhysicalDeviceMultiviewFeatures* features) const
{
	features->multiview = VK_FALSE;
	features->multiviewGeometryShader = VK_FALSE;
	features->multiviewTessellationShader = VK_FALSE;
}

void PhysicalDevice::getFeatures(VkPhysicalDeviceProtectedMemoryFeatures* features) const
{
	features->protectedMemory = VK_FALSE;
}

VkSampleCountFlags PhysicalDevice::getSampleCounts() const
{
	return VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT;
}

const VkPhysicalDeviceLimits& PhysicalDevice::getLimits() const
{
	VkSampleCountFlags sampleCounts = getSampleCounts();

	static const VkPhysicalDeviceLimits limits =
	{
		(1 << vk::MAX_IMAGE_LEVELS_1D), // maxImageDimension1D
		(1 << vk::MAX_IMAGE_LEVELS_2D), // maxImageDimension2D
		(1 << vk::MAX_IMAGE_LEVELS_3D), // maxImageDimension3D
		(1 << vk::MAX_IMAGE_LEVELS_CUBE), // maxImageDimensionCube
		(1 << vk::MAX_IMAGE_ARRAY_LAYERS), // maxImageArrayLayers
		65536, // maxTexelBufferElements
		16384, // maxUniformBufferRange
		(1ul << 27), // maxStorageBufferRange
		128, // maxPushConstantsSize
		4096, // maxMemoryAllocationCount
		4000, // maxSamplerAllocationCount
		131072, // bufferImageGranularity
		0, // sparseAddressSpaceSize (unsupported)
		4, // maxBoundDescriptorSets
		16, // maxPerStageDescriptorSamplers
		12, // maxPerStageDescriptorUniformBuffers
		4, // maxPerStageDescriptorStorageBuffers
		16, // maxPerStageDescriptorSampledImages
		4, // maxPerStageDescriptorStorageImages
		4, // maxPerStageDescriptorInputAttachments
		128, // maxPerStageResources
		96, // maxDescriptorSetSamplers
		72, // maxDescriptorSetUniformBuffers
		8, // maxDescriptorSetUniformBuffersDynamic
		24, // maxDescriptorSetStorageBuffers
		4, // maxDescriptorSetStorageBuffersDynamic
		96, // maxDescriptorSetSampledImages
		24, // maxDescriptorSetStorageImages
		4, // maxDescriptorSetInputAttachments
		16, // maxVertexInputAttributes
		vk::MAX_VERTEX_INPUT_BINDINGS, // maxVertexInputBindings
		2047, // maxVertexInputAttributeOffset
		2048, // maxVertexInputBindingStride
		64, // maxVertexOutputComponents
		0, // maxTessellationGenerationLevel (unsupported)
		0, // maxTessellationPatchSize (unsupported)
		0, // maxTessellationControlPerVertexInputComponents (unsupported)
		0, // maxTessellationControlPerVertexOutputComponents (unsupported)
		0, // maxTessellationControlPerPatchOutputComponents (unsupported)
		0, // maxTessellationControlTotalOutputComponents (unsupported)
		0, // maxTessellationEvaluationInputComponents (unsupported)
		0, // maxTessellationEvaluationOutputComponents (unsupported)
		0, // maxGeometryShaderInvocations (unsupported)
		0, // maxGeometryInputComponents (unsupported)
		0, // maxGeometryOutputComponents (unsupported)
		0, // maxGeometryOutputVertices (unsupported)
		0, // maxGeometryTotalOutputComponents (unsupported)
		64, // maxFragmentInputComponents
		4, // maxFragmentOutputAttachments
		1, // maxFragmentDualSrcAttachments
		4, // maxFragmentCombinedOutputResources
		16384, // maxComputeSharedMemorySize
		{ 65535, 65535, 65535 }, // maxComputeWorkGroupCount[3]
		128, // maxComputeWorkGroupInvocations
		{ 128, 128, 64, }, // maxComputeWorkGroupSize[3]
		4, // subPixelPrecisionBits
		4, // subTexelPrecisionBits
		4, // mipmapPrecisionBits
		UINT32_MAX, // maxDrawIndexedIndexValue
		UINT32_MAX, // maxDrawIndirectCount
		2, // maxSamplerLodBias
		16, // maxSamplerAnisotropy
		16, // maxViewports
		{ 4096, 4096 }, // maxViewportDimensions[2]
		{ -8192, 8191 }, // viewportBoundsRange[2]
		0, // viewportSubPixelBits
		64, // minMemoryMapAlignment
		256, // minTexelBufferOffsetAlignment
		256, // minUniformBufferOffsetAlignment
		256, // minStorageBufferOffsetAlignment
		-8, // minTexelOffset
		7, // maxTexelOffset
		-8, // minTexelGatherOffset
		7, // maxTexelGatherOffset
		-0.5, // minInterpolationOffset
		0.5, // maxInterpolationOffset
		4, // subPixelInterpolationOffsetBits
		4096, // maxFramebufferWidth
		4096, // maxFramebufferHeight
		256, // maxFramebufferLayers
		sampleCounts, // framebufferColorSampleCounts
		sampleCounts, // framebufferDepthSampleCounts
		sampleCounts, // framebufferStencilSampleCounts
		sampleCounts, // framebufferNoAttachmentsSampleCounts
		4,  // maxColorAttachments
		sampleCounts, // sampledImageColorSampleCounts
		VK_SAMPLE_COUNT_1_BIT, // sampledImageIntegerSampleCounts
		sampleCounts, // sampledImageDepthSampleCounts
		sampleCounts, // sampledImageStencilSampleCounts
		VK_SAMPLE_COUNT_1_BIT, // storageImageSampleCounts (unsupported)
		1, // maxSampleMaskWords
		false, // timestampComputeAndGraphics
		60, // timestampPeriod
		8, // maxClipDistances
		8, // maxCullDistances
		8, // maxCombinedClipAndCullDistances
		2, // discreteQueuePriorities
		{ 1.0, 64.0 }, // pointSizeRange[2]
		{ 1.0, 1.0 }, // lineWidthRange[2] (unsupported)
		0.0, // pointSizeGranularity (unsupported)
		0.0, // lineWidthGranularity (unsupported)
		false, // strictLines
		true, // standardSampleLocations
		64, // optimalBufferCopyOffsetAlignment
		64, // optimalBufferCopyRowPitchAlignment
		256, // nonCoherentAtomSize
	};

	return limits;
}

const VkPhysicalDeviceProperties& PhysicalDevice::getProperties() const
{
	static const VkPhysicalDeviceProperties properties
	{
		API_VERSION,
		DRIVER_VERSION,
		VENDOR_ID,
		DEVICE_ID,
		VK_PHYSICAL_DEVICE_TYPE_CPU, // deviceType
		SWIFTSHADER_DEVICE_NAME, // deviceName
		SWIFTSHADER_UUID, // pipelineCacheUUID
		getLimits(), // limits
		{ 0 } // sparseProperties
	};

	return properties;
}

void PhysicalDevice::getProperties(VkPhysicalDeviceIDProperties* properties) const
{
	memset(properties->deviceUUID, 0, VK_UUID_SIZE);
	memset(properties->driverUUID, 0, VK_UUID_SIZE);
	memset(properties->deviceLUID, 0, VK_LUID_SIZE);

	memcpy(properties->deviceUUID, SWIFTSHADER_UUID, VK_UUID_SIZE);
	*((uint64_t*)properties->driverUUID) = DRIVER_VERSION;

	properties->deviceNodeMask = 0;
	properties->deviceLUIDValid = VK_FALSE;
}

void PhysicalDevice::getProperties(VkPhysicalDeviceMaintenance3Properties* properties) const
{
	properties->maxMemoryAllocationSize = 1 << 31;
	properties->maxPerSetDescriptors = 1024;
}

void PhysicalDevice::getProperties(VkPhysicalDeviceMultiviewProperties* properties) const
{
	properties->maxMultiviewInstanceIndex = (1 << 27) - 1;
	properties->maxMultiviewViewCount = 6;
}

void PhysicalDevice::getProperties(VkPhysicalDevicePointClippingProperties* properties) const
{
	properties->pointClippingBehavior = VK_POINT_CLIPPING_BEHAVIOR_ALL_CLIP_PLANES;
}

void PhysicalDevice::getProperties(VkPhysicalDeviceProtectedMemoryProperties* properties) const
{
	properties->protectedNoFault = VK_FALSE;
}

void PhysicalDevice::getProperties(VkPhysicalDeviceSubgroupProperties* properties) const
{
	properties->subgroupSize = 1;
	properties->supportedStages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;
	properties->supportedOperations = VK_SUBGROUP_FEATURE_BASIC_BIT;
	properties->quadOperationsInAllStages = VK_FALSE;
}

bool PhysicalDevice::hasFeatures(const VkPhysicalDeviceFeatures& requestedFeatures) const
{
	const VkPhysicalDeviceFeatures& availableFeatures = getFeatures();

	return (!requestedFeatures.robustBufferAccess || availableFeatures.robustBufferAccess) &&
	       (!requestedFeatures.fullDrawIndexUint32 || availableFeatures.fullDrawIndexUint32) &&
	       (!requestedFeatures.imageCubeArray || availableFeatures.imageCubeArray) &&
	       (!requestedFeatures.independentBlend || availableFeatures.independentBlend) &&
	       (!requestedFeatures.geometryShader || availableFeatures.geometryShader) &&
	       (!requestedFeatures.tessellationShader || availableFeatures.tessellationShader) &&
	       (!requestedFeatures.sampleRateShading || availableFeatures.sampleRateShading) &&
	       (!requestedFeatures.dualSrcBlend || availableFeatures.dualSrcBlend) &&
	       (!requestedFeatures.logicOp || availableFeatures.logicOp) &&
	       (!requestedFeatures.multiDrawIndirect || availableFeatures.multiDrawIndirect) &&
	       (!requestedFeatures.drawIndirectFirstInstance || availableFeatures.drawIndirectFirstInstance) &&
	       (!requestedFeatures.depthClamp || availableFeatures.depthClamp) &&
	       (!requestedFeatures.depthBiasClamp || availableFeatures.depthBiasClamp) &&
	       (!requestedFeatures.fillModeNonSolid || availableFeatures.fillModeNonSolid) &&
	       (!requestedFeatures.depthBounds || availableFeatures.depthBounds) &&
	       (!requestedFeatures.wideLines || availableFeatures.wideLines) &&
	       (!requestedFeatures.largePoints || availableFeatures.largePoints) &&
	       (!requestedFeatures.alphaToOne || availableFeatures.alphaToOne) &&
	       (!requestedFeatures.multiViewport || availableFeatures.multiViewport) &&
	       (!requestedFeatures.samplerAnisotropy || availableFeatures.samplerAnisotropy) &&
	       (!requestedFeatures.textureCompressionETC2 || availableFeatures.textureCompressionETC2) &&
	       (!requestedFeatures.textureCompressionASTC_LDR || availableFeatures.textureCompressionASTC_LDR) &&
	       (!requestedFeatures.textureCompressionBC || availableFeatures.textureCompressionBC) &&
	       (!requestedFeatures.occlusionQueryPrecise || availableFeatures.occlusionQueryPrecise) &&
	       (!requestedFeatures.pipelineStatisticsQuery || availableFeatures.pipelineStatisticsQuery) &&
	       (!requestedFeatures.vertexPipelineStoresAndAtomics || availableFeatures.vertexPipelineStoresAndAtomics) &&
	       (!requestedFeatures.fragmentStoresAndAtomics || availableFeatures.fragmentStoresAndAtomics) &&
	       (!requestedFeatures.shaderTessellationAndGeometryPointSize || availableFeatures.shaderTessellationAndGeometryPointSize) &&
	       (!requestedFeatures.shaderImageGatherExtended || availableFeatures.shaderImageGatherExtended) &&
	       (!requestedFeatures.shaderStorageImageExtendedFormats || availableFeatures.shaderStorageImageExtendedFormats) &&
	       (!requestedFeatures.shaderStorageImageMultisample || availableFeatures.shaderStorageImageMultisample) &&
	       (!requestedFeatures.shaderStorageImageReadWithoutFormat || availableFeatures.shaderStorageImageReadWithoutFormat) &&
	       (!requestedFeatures.shaderStorageImageWriteWithoutFormat || availableFeatures.shaderStorageImageWriteWithoutFormat) &&
	       (!requestedFeatures.shaderUniformBufferArrayDynamicIndexing || availableFeatures.shaderUniformBufferArrayDynamicIndexing) &&
	       (!requestedFeatures.shaderSampledImageArrayDynamicIndexing || availableFeatures.shaderSampledImageArrayDynamicIndexing) &&
	       (!requestedFeatures.shaderStorageBufferArrayDynamicIndexing || availableFeatures.shaderStorageBufferArrayDynamicIndexing) &&
	       (!requestedFeatures.shaderStorageImageArrayDynamicIndexing || availableFeatures.shaderStorageImageArrayDynamicIndexing) &&
	       (!requestedFeatures.shaderClipDistance || availableFeatures.shaderClipDistance) &&
	       (!requestedFeatures.shaderCullDistance || availableFeatures.shaderCullDistance) &&
	       (!requestedFeatures.shaderFloat64 || availableFeatures.shaderFloat64) &&
	       (!requestedFeatures.shaderInt64 || availableFeatures.shaderInt64) &&
	       (!requestedFeatures.shaderInt16 || availableFeatures.shaderInt16) &&
	       (!requestedFeatures.shaderResourceResidency || availableFeatures.shaderResourceResidency) &&
	       (!requestedFeatures.shaderResourceMinLod || availableFeatures.shaderResourceMinLod) &&
	       (!requestedFeatures.sparseBinding || availableFeatures.sparseBinding) &&
	       (!requestedFeatures.sparseResidencyBuffer || availableFeatures.sparseResidencyBuffer) &&
	       (!requestedFeatures.sparseResidencyImage2D || availableFeatures.sparseResidencyImage2D) &&
	       (!requestedFeatures.sparseResidencyImage3D || availableFeatures.sparseResidencyImage3D) &&
	       (!requestedFeatures.sparseResidency2Samples || availableFeatures.sparseResidency2Samples) &&
	       (!requestedFeatures.sparseResidency4Samples || availableFeatures.sparseResidency4Samples) &&
	       (!requestedFeatures.sparseResidency8Samples || availableFeatures.sparseResidency8Samples) &&
	       (!requestedFeatures.sparseResidency16Samples || availableFeatures.sparseResidency16Samples) &&
	       (!requestedFeatures.sparseResidencyAliased || availableFeatures.sparseResidencyAliased) &&
	       (!requestedFeatures.variableMultisampleRate || availableFeatures.variableMultisampleRate) &&
	       (!requestedFeatures.inheritedQueries || availableFeatures.inheritedQueries);
}

void PhysicalDevice::getFormatProperties(VkFormat format, VkFormatProperties* pFormatProperties) const
{
	pFormatProperties->linearTilingFeatures = 0; // Unsupported format
	pFormatProperties->optimalTilingFeatures = 0; // Unsupported format
	pFormatProperties->bufferFeatures = 0; // Unsupported format
}

void PhysicalDevice::getImageFormatProperties(VkFormat format, VkImageType type, VkImageTiling tiling,
                                              VkImageUsageFlags usage, VkImageCreateFlags flags,
	                                          VkImageFormatProperties* pImageFormatProperties) const
{
	pImageFormatProperties->maxArrayLayers = 1 << vk::MAX_IMAGE_ARRAY_LAYERS;

	switch(type)
	{
	case VK_IMAGE_TYPE_1D:
		pImageFormatProperties->maxMipLevels = vk::MAX_IMAGE_LEVELS_1D;
		pImageFormatProperties->maxExtent.width = 1 << vk::MAX_IMAGE_LEVELS_1D;
		pImageFormatProperties->maxExtent.height = 1;
		pImageFormatProperties->maxExtent.depth = 1;
		break;
	case VK_IMAGE_TYPE_2D:
		if(flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
		{
			pImageFormatProperties->maxMipLevels = vk::MAX_IMAGE_LEVELS_CUBE;
			pImageFormatProperties->maxExtent.width = 1 << vk::MAX_IMAGE_LEVELS_CUBE;
			pImageFormatProperties->maxExtent.height = 1 << vk::MAX_IMAGE_LEVELS_CUBE;
			pImageFormatProperties->maxExtent.depth = 1;
		}
		else
		{
			pImageFormatProperties->maxMipLevels = vk::MAX_IMAGE_LEVELS_2D;
			pImageFormatProperties->maxExtent.width = 1 << vk::MAX_IMAGE_LEVELS_2D;
			pImageFormatProperties->maxExtent.height = 1 << vk::MAX_IMAGE_LEVELS_2D;
			pImageFormatProperties->maxExtent.depth = 1;
		}
		break;
	case VK_IMAGE_TYPE_3D:
		pImageFormatProperties->maxMipLevels = vk::MAX_IMAGE_LEVELS_3D;
		pImageFormatProperties->maxExtent.width = 1 << vk::MAX_IMAGE_LEVELS_3D;
		pImageFormatProperties->maxExtent.height = 1 << vk::MAX_IMAGE_LEVELS_3D;
		pImageFormatProperties->maxExtent.depth = 1 << vk::MAX_IMAGE_LEVELS_3D;
		break;
	default:
		UNREACHABLE(type);
		break;
	}

	pImageFormatProperties->maxResourceSize = 1 << 31; // Minimum value for maxResourceSize
	pImageFormatProperties->sampleCounts = getSampleCounts();

}

uint32_t PhysicalDevice::getQueueFamilyPropertyCount() const
{
	return 1;
}

void PhysicalDevice::getQueueFamilyProperties(uint32_t pQueueFamilyPropertyCount,
                                              VkQueueFamilyProperties* pQueueFamilyProperties) const
{
	for(uint32_t i = 0; i < pQueueFamilyPropertyCount; i++)
	{
		pQueueFamilyProperties[i].minImageTransferGranularity.width = 1;
		pQueueFamilyProperties[i].minImageTransferGranularity.height = 1;
		pQueueFamilyProperties[i].minImageTransferGranularity.depth = 1;
		pQueueFamilyProperties[i].queueCount = 1;
		pQueueFamilyProperties[i].queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
		pQueueFamilyProperties[i].timestampValidBits = 0; // No support for time stamps
	}
}

const VkPhysicalDeviceMemoryProperties& PhysicalDevice::getMemoryProperties() const
{
	static const VkPhysicalDeviceMemoryProperties properties
	{
		1, // memoryTypeCount
		{{VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
		  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		  0}}, // heapIndex
		1, // memoryHeapCount
		{{1ull << 31, // size, FIXME(sugoi): This could be configurable based on available RAM
		  VK_MEMORY_HEAP_DEVICE_LOCAL_BIT}},
	};

	return properties;
}

void PhysicalDevice::getExternalBufferProperties(const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo,
                                                 VkExternalBufferProperties* pExternalBufferProperties) const
{
	// FIXME: currently ignoring pExternalBufferInfo

	pExternalBufferProperties->externalMemoryProperties.compatibleHandleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;
	pExternalBufferProperties->externalMemoryProperties.exportFromImportedHandleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;
	pExternalBufferProperties->externalMemoryProperties.externalMemoryFeatures = VK_EXTERNAL_MEMORY_FEATURE_DEDICATED_ONLY_BIT;
}

void PhysicalDevice::getExternalFenceProperties(const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo,
                                                VkExternalFenceProperties* pExternalFenceProperties) const
{
	// FIXME: currently ignoring pExternalFenceInfo

	pExternalFenceProperties->compatibleHandleTypes = VK_EXTERNAL_FENCE_HANDLE_TYPE_OPAQUE_FD_BIT;
	pExternalFenceProperties->exportFromImportedHandleTypes = VK_EXTERNAL_FENCE_HANDLE_TYPE_OPAQUE_FD_BIT;
	pExternalFenceProperties->externalFenceFeatures = VK_EXTERNAL_FENCE_FEATURE_EXPORTABLE_BIT;
}

void PhysicalDevice::getExternalSemaphoreProperties(const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo,
                                                    VkExternalSemaphoreProperties* pExternalSemaphoreProperties) const
{
	// FIXME: currently ignoring pExternalSemaphoreInfo

	pExternalSemaphoreProperties->compatibleHandleTypes = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT;
	pExternalSemaphoreProperties->exportFromImportedHandleTypes = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT;
	pExternalSemaphoreProperties->externalSemaphoreFeatures = VK_EXTERNAL_SEMAPHORE_FEATURE_EXPORTABLE_BIT;
}

} // namespace vk
