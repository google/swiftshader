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

#include "VkConfig.hpp"
#include "VkStringify.hpp"
#include "Pipeline/SpirvShader.hpp"  // sw::SIMD::Width
#include "Reactor/Reactor.hpp"

#include <cstring>
#include <limits>

namespace vk {

static void setExternalMemoryProperties(VkExternalMemoryHandleTypeFlagBits handleType, VkExternalMemoryProperties *properties)
{
#if SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD
	if(handleType == VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT)
	{
		properties->compatibleHandleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;
		properties->exportFromImportedHandleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;
		properties->externalMemoryFeatures = VK_EXTERNAL_MEMORY_FEATURE_IMPORTABLE_BIT | VK_EXTERNAL_MEMORY_FEATURE_EXPORTABLE_BIT;
		return;
	}
#endif
#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
	if(handleType == VK_EXTERNAL_MEMORY_HANDLE_TYPE_ANDROID_HARDWARE_BUFFER_BIT_ANDROID)
	{
		properties->compatibleHandleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_ANDROID_HARDWARE_BUFFER_BIT_ANDROID;
		properties->exportFromImportedHandleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_ANDROID_HARDWARE_BUFFER_BIT_ANDROID;
		properties->externalMemoryFeatures = VK_EXTERNAL_MEMORY_FEATURE_IMPORTABLE_BIT | VK_EXTERNAL_MEMORY_FEATURE_EXPORTABLE_BIT;
		return;
	}
#endif
#if VK_USE_PLATFORM_FUCHSIA
	if(handleType == VK_EXTERNAL_MEMORY_HANDLE_TYPE_TEMP_ZIRCON_VMO_BIT_FUCHSIA)
	{
		properties->compatibleHandleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_TEMP_ZIRCON_VMO_BIT_FUCHSIA;
		properties->exportFromImportedHandleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_TEMP_ZIRCON_VMO_BIT_FUCHSIA;
		properties->externalMemoryFeatures = VK_EXTERNAL_MEMORY_FEATURE_IMPORTABLE_BIT | VK_EXTERNAL_MEMORY_FEATURE_EXPORTABLE_BIT;
		return;
	}
#endif
	properties->compatibleHandleTypes = 0;
	properties->exportFromImportedHandleTypes = 0;
	properties->externalMemoryFeatures = 0;
}

PhysicalDevice::PhysicalDevice(const void *, void *mem)
{
}

const VkPhysicalDeviceFeatures &PhysicalDevice::getFeatures() const
{
	static const VkPhysicalDeviceFeatures features{
		VK_TRUE,   // robustBufferAccess
		VK_TRUE,   // fullDrawIndexUint32
		VK_TRUE,   // imageCubeArray
		VK_TRUE,   // independentBlend
		VK_FALSE,  // geometryShader
		VK_FALSE,  // tessellationShader
		VK_FALSE,  // sampleRateShading
		VK_FALSE,  // dualSrcBlend
		VK_FALSE,  // logicOp
		VK_TRUE,   // multiDrawIndirect
		VK_TRUE,   // drawIndirectFirstInstance
		VK_FALSE,  // depthClamp
		VK_TRUE,   // depthBiasClamp
		VK_TRUE,   // fillModeNonSolid
		VK_FALSE,  // depthBounds
		VK_FALSE,  // wideLines
		VK_TRUE,   // largePoints
		VK_FALSE,  // alphaToOne
		VK_FALSE,  // multiViewport
		VK_TRUE,   // samplerAnisotropy
		VK_TRUE,   // textureCompressionETC2
#ifdef SWIFTSHADER_ENABLE_ASTC
		VK_TRUE,  // textureCompressionASTC_LDR
#else
		VK_FALSE,  // textureCompressionASTC_LDR
#endif
		VK_TRUE,   // textureCompressionBC
		VK_TRUE,   // occlusionQueryPrecise
		VK_FALSE,  // pipelineStatisticsQuery
		VK_TRUE,   // vertexPipelineStoresAndAtomics
		VK_TRUE,   // fragmentStoresAndAtomics
		VK_FALSE,  // shaderTessellationAndGeometryPointSize
		VK_FALSE,  // shaderImageGatherExtended
		VK_TRUE,   // shaderStorageImageExtendedFormats
		VK_TRUE,   // shaderStorageImageMultisample
		VK_FALSE,  // shaderStorageImageReadWithoutFormat
		VK_FALSE,  // shaderStorageImageWriteWithoutFormat
		VK_TRUE,   // shaderUniformBufferArrayDynamicIndexing
		VK_TRUE,   // shaderSampledImageArrayDynamicIndexing
		VK_TRUE,   // shaderStorageBufferArrayDynamicIndexing
		VK_TRUE,   // shaderStorageImageArrayDynamicIndexing
		VK_TRUE,   // shaderClipDistance
		VK_TRUE,   // shaderCullDistance
		VK_FALSE,  // shaderFloat64
		VK_FALSE,  // shaderInt64
		VK_FALSE,  // shaderInt16
		VK_FALSE,  // shaderResourceResidency
		VK_FALSE,  // shaderResourceMinLod
		VK_FALSE,  // sparseBinding
		VK_FALSE,  // sparseResidencyBuffer
		VK_FALSE,  // sparseResidencyImage2D
		VK_FALSE,  // sparseResidencyImage3D
		VK_FALSE,  // sparseResidency2Samples
		VK_FALSE,  // sparseResidency4Samples
		VK_FALSE,  // sparseResidency8Samples
		VK_FALSE,  // sparseResidency16Samples
		VK_FALSE,  // sparseResidencyAliased
		VK_TRUE,   // variableMultisampleRate
		VK_FALSE,  // inheritedQueries
	};

	return features;
}

template<typename T>
static void getPhysicalDeviceSamplerYcbcrConversionFeatures(T *features)
{
	features->samplerYcbcrConversion = VK_TRUE;
}

template<typename T>
static void getPhysicalDevice16BitStorageFeatures(T *features)
{
	features->storageBuffer16BitAccess = VK_FALSE;
	features->storageInputOutput16 = VK_FALSE;
	features->storagePushConstant16 = VK_FALSE;
	features->uniformAndStorageBuffer16BitAccess = VK_FALSE;
}

template<typename T>
static void getPhysicalDeviceVariablePointersFeatures(T *features)
{
	features->variablePointersStorageBuffer = VK_FALSE;
	features->variablePointers = VK_FALSE;
}

template<typename T>
static void getPhysicalDevice8BitStorageFeaturesKHR(T *features)
{
	features->storageBuffer8BitAccess = VK_FALSE;
	features->uniformAndStorageBuffer8BitAccess = VK_FALSE;
	features->storagePushConstant8 = VK_FALSE;
}

template<typename T>
static void getPhysicalDeviceMultiviewFeatures(T *features)
{
	features->multiview = VK_TRUE;
	features->multiviewGeometryShader = VK_FALSE;
	features->multiviewTessellationShader = VK_FALSE;
}

template<typename T>
static void getPhysicalDeviceProtectedMemoryFeatures(T *features)
{
	features->protectedMemory = VK_FALSE;
}

template<typename T>
static void getPhysicalDeviceShaderDrawParameterFeatures(T *features)
{
	features->shaderDrawParameters = VK_FALSE;
}

template<typename T>
static void getPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR(T *features)
{
	features->separateDepthStencilLayouts = VK_TRUE;
}

template<typename T>
static void getPhysicalDeviceLineRasterizationFeaturesEXT(T *features)
{
	features->rectangularLines = VK_TRUE;
	features->bresenhamLines = VK_TRUE;
	features->smoothLines = VK_FALSE;
	features->stippledRectangularLines = VK_FALSE;
	features->stippledBresenhamLines = VK_FALSE;
	features->stippledSmoothLines = VK_FALSE;
}

template<typename T>
static void getPhysicalDeviceProvokingVertexFeaturesEXT(T *features)
{
	features->provokingVertexLast = VK_TRUE;
}

template<typename T>
static void getPhysicalDeviceImageRobustnessFeaturesEXT(T *features)
{
	features->robustImageAccess = VK_TRUE;
}

template<typename T>
static void getPhysicalDeviceShaderDrawParametersFeatures(T *features)
{
	features->shaderDrawParameters = VK_FALSE;
}

template<typename T>
static void getPhysicalDeviceVulkan11Features(T *features)
{
	getPhysicalDevice16BitStorageFeatures(features);
	getPhysicalDeviceMultiviewFeatures(features);
	getPhysicalDeviceVariablePointersFeatures(features);
	getPhysicalDeviceProtectedMemoryFeatures(features);
	getPhysicalDeviceSamplerYcbcrConversionFeatures(features);
	getPhysicalDeviceShaderDrawParametersFeatures(features);
}

template<typename T>
static void getPhysicalDeviceImagelessFramebufferFeatures(T *features)
{
	features->imagelessFramebuffer = VK_TRUE;
}

template<typename T>
static void getPhysicalDeviceVulkan12Features(T *features)
{
	features->samplerMirrorClampToEdge = VK_FALSE;
	features->drawIndirectCount = VK_FALSE;
	getPhysicalDevice8BitStorageFeaturesKHR(features);
	features->shaderBufferInt64Atomics = VK_FALSE;
	features->shaderSharedInt64Atomics = VK_FALSE;
	features->shaderFloat16 = VK_FALSE;
	features->shaderInt8 = VK_FALSE;
	features->descriptorIndexing = VK_FALSE;
	features->shaderInputAttachmentArrayDynamicIndexing = VK_FALSE;
	features->shaderUniformTexelBufferArrayDynamicIndexing = VK_FALSE;
	features->shaderStorageTexelBufferArrayDynamicIndexing = VK_FALSE;
	features->shaderSampledImageArrayNonUniformIndexing = VK_FALSE;
	features->shaderStorageBufferArrayNonUniformIndexing = VK_FALSE;
	features->shaderStorageImageArrayNonUniformIndexing = VK_FALSE;
	features->shaderInputAttachmentArrayNonUniformIndexing = VK_FALSE;
	features->shaderUniformTexelBufferArrayNonUniformIndexing = VK_FALSE;
	features->shaderStorageTexelBufferArrayNonUniformIndexing = VK_FALSE;
	features->descriptorBindingUniformBufferUpdateAfterBind = VK_FALSE;
	features->descriptorBindingSampledImageUpdateAfterBind = VK_FALSE;
	features->descriptorBindingStorageImageUpdateAfterBind = VK_FALSE;
	features->descriptorBindingStorageBufferUpdateAfterBind = VK_FALSE;
	features->descriptorBindingUniformTexelBufferUpdateAfterBind = VK_FALSE;
	features->descriptorBindingStorageBufferUpdateAfterBind = VK_FALSE;
	features->descriptorBindingUpdateUnusedWhilePending = VK_FALSE;
	features->descriptorBindingPartiallyBound = VK_FALSE;
	features->descriptorBindingVariableDescriptorCount = VK_FALSE;
	features->runtimeDescriptorArray = VK_FALSE;
	features->samplerFilterMinmax = VK_FALSE;
	features->scalarBlockLayout = VK_FALSE;
	getPhysicalDeviceImagelessFramebufferFeatures(features);
	features->uniformBufferStandardLayout = VK_FALSE;
	features->shaderSubgroupExtendedTypes = VK_FALSE;
	getPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR(features);
	features->hostQueryReset = VK_FALSE;
	features->timelineSemaphore = VK_FALSE;
	features->bufferDeviceAddress = VK_FALSE;
	features->bufferDeviceAddressCaptureReplay = VK_FALSE;
	features->bufferDeviceAddressMultiDevice = VK_FALSE;
	features->vulkanMemoryModel = VK_FALSE;
	features->vulkanMemoryModelDeviceScope = VK_FALSE;
	features->vulkanMemoryModelAvailabilityVisibilityChains = VK_FALSE;
	features->shaderOutputViewportIndex = VK_FALSE;
	features->shaderOutputLayer = VK_FALSE;
	features->subgroupBroadcastDynamicId = VK_FALSE;
}

void PhysicalDevice::getFeatures2(VkPhysicalDeviceFeatures2 *features) const
{
	features->features = getFeatures();
	VkBaseOutStructure *curExtension = reinterpret_cast<VkBaseOutStructure *>(features->pNext);
	while(curExtension != nullptr)
	{
		// Need to switch on an integer since Provoking Vertex isn't a part of the Vulkan spec.
		switch((int)curExtension->sType)
		{
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES:
				getPhysicalDeviceVulkan11Features(reinterpret_cast<VkPhysicalDeviceVulkan11Features *>(curExtension));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES:
				getPhysicalDeviceVulkan12Features(reinterpret_cast<VkPhysicalDeviceVulkan12Features *>(curExtension));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES:
				getPhysicalDeviceMultiviewFeatures(reinterpret_cast<VkPhysicalDeviceMultiviewFeatures *>(curExtension));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES:
				getPhysicalDeviceVariablePointersFeatures(reinterpret_cast<VkPhysicalDeviceVariablePointersFeatures *>(curExtension));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES:
				getPhysicalDevice16BitStorageFeatures(reinterpret_cast<VkPhysicalDevice16BitStorageFeatures *>(curExtension));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES:
				getPhysicalDeviceSamplerYcbcrConversionFeatures(reinterpret_cast<VkPhysicalDeviceSamplerYcbcrConversionFeatures *>(curExtension));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES:
				getPhysicalDeviceProtectedMemoryFeatures(reinterpret_cast<VkPhysicalDeviceProtectedMemoryFeatures *>(curExtension));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES:
				getPhysicalDeviceShaderDrawParameterFeatures(reinterpret_cast<VkPhysicalDeviceShaderDrawParameterFeatures *>(curExtension));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT:
				getPhysicalDeviceImageRobustnessFeaturesEXT(reinterpret_cast<VkPhysicalDeviceImageRobustnessFeaturesEXT *>(curExtension));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT:
				getPhysicalDeviceLineRasterizationFeaturesEXT(reinterpret_cast<VkPhysicalDeviceLineRasterizationFeaturesEXT *>(curExtension));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES:
				getPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR(reinterpret_cast<VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures *>(curExtension));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR:
				getPhysicalDevice8BitStorageFeaturesKHR(reinterpret_cast<VkPhysicalDevice8BitStorageFeaturesKHR *>(curExtension));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT:
				getPhysicalDeviceProvokingVertexFeaturesEXT(reinterpret_cast<VkPhysicalDeviceProvokingVertexFeaturesEXT *>(curExtension));
				break;
			case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES:
				getPhysicalDeviceImagelessFramebufferFeatures(reinterpret_cast<VkPhysicalDeviceImagelessFramebufferFeatures *>(curExtension));
				break;
			default:
				WARN("curExtension->pNext->sType = %s", vk::Stringify(curExtension->sType).c_str());
				break;
		}
		curExtension = reinterpret_cast<VkBaseOutStructure *>(curExtension->pNext);
	}
}

VkSampleCountFlags PhysicalDevice::getSampleCounts() const
{
	return VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT;
}

const VkPhysicalDeviceLimits &PhysicalDevice::getLimits() const
{
	VkSampleCountFlags sampleCounts = getSampleCounts();

	static const VkPhysicalDeviceLimits limits = {
		1 << (vk::MAX_IMAGE_LEVELS_1D - 1),               // maxImageDimension1D
		1 << (vk::MAX_IMAGE_LEVELS_2D - 1),               // maxImageDimension2D
		1 << (vk::MAX_IMAGE_LEVELS_3D - 1),               // maxImageDimension3D
		1 << (vk::MAX_IMAGE_LEVELS_CUBE - 1),             // maxImageDimensionCube
		vk::MAX_IMAGE_ARRAY_LAYERS,                       // maxImageArrayLayers
		65536,                                            // maxTexelBufferElements
		16384,                                            // maxUniformBufferRange
		(1ul << 27),                                      // maxStorageBufferRange
		vk::MAX_PUSH_CONSTANT_SIZE,                       // maxPushConstantsSize
		4096,                                             // maxMemoryAllocationCount
		4000,                                             // maxSamplerAllocationCount
		131072,                                           // bufferImageGranularity
		0,                                                // sparseAddressSpaceSize (unsupported)
		MAX_BOUND_DESCRIPTOR_SETS,                        // maxBoundDescriptorSets
		16,                                               // maxPerStageDescriptorSamplers
		14,                                               // maxPerStageDescriptorUniformBuffers
		16,                                               // maxPerStageDescriptorStorageBuffers
		16,                                               // maxPerStageDescriptorSampledImages
		4,                                                // maxPerStageDescriptorStorageImages
		4,                                                // maxPerStageDescriptorInputAttachments
		128,                                              // maxPerStageResources
		96,                                               // maxDescriptorSetSamplers
		72,                                               // maxDescriptorSetUniformBuffers
		MAX_DESCRIPTOR_SET_UNIFORM_BUFFERS_DYNAMIC,       // maxDescriptorSetUniformBuffersDynamic
		24,                                               // maxDescriptorSetStorageBuffers
		MAX_DESCRIPTOR_SET_STORAGE_BUFFERS_DYNAMIC,       // maxDescriptorSetStorageBuffersDynamic
		96,                                               // maxDescriptorSetSampledImages
		24,                                               // maxDescriptorSetStorageImages
		4,                                                // maxDescriptorSetInputAttachments
		16,                                               // maxVertexInputAttributes
		vk::MAX_VERTEX_INPUT_BINDINGS,                    // maxVertexInputBindings
		2047,                                             // maxVertexInputAttributeOffset
		2048,                                             // maxVertexInputBindingStride
		sw::MAX_INTERFACE_COMPONENTS,                     // maxVertexOutputComponents
		0,                                                // maxTessellationGenerationLevel (unsupported)
		0,                                                // maxTessellationPatchSize (unsupported)
		0,                                                // maxTessellationControlPerVertexInputComponents (unsupported)
		0,                                                // maxTessellationControlPerVertexOutputComponents (unsupported)
		0,                                                // maxTessellationControlPerPatchOutputComponents (unsupported)
		0,                                                // maxTessellationControlTotalOutputComponents (unsupported)
		0,                                                // maxTessellationEvaluationInputComponents (unsupported)
		0,                                                // maxTessellationEvaluationOutputComponents (unsupported)
		0,                                                // maxGeometryShaderInvocations (unsupported)
		0,                                                // maxGeometryInputComponents (unsupported)
		0,                                                // maxGeometryOutputComponents (unsupported)
		0,                                                // maxGeometryOutputVertices (unsupported)
		0,                                                // maxGeometryTotalOutputComponents (unsupported)
		sw::MAX_INTERFACE_COMPONENTS,                     // maxFragmentInputComponents
		4,                                                // maxFragmentOutputAttachments
		1,                                                // maxFragmentDualSrcAttachments
		4,                                                // maxFragmentCombinedOutputResources
		16384,                                            // maxComputeSharedMemorySize
		{ 65535, 65535, 65535 },                          // maxComputeWorkGroupCount[3]
		128,                                              // maxComputeWorkGroupInvocations
		{ 128, 128, 64 },                                 // maxComputeWorkGroupSize[3]
		vk::SUBPIXEL_PRECISION_BITS,                      // subPixelPrecisionBits
		4,                                                // subTexelPrecisionBits
		4,                                                // mipmapPrecisionBits
		UINT32_MAX,                                       // maxDrawIndexedIndexValue
		UINT32_MAX,                                       // maxDrawIndirectCount
		vk::MAX_SAMPLER_LOD_BIAS,                         // maxSamplerLodBias
		16,                                               // maxSamplerAnisotropy
		16,                                               // maxViewports
		{ 4096, 4096 },                                   // maxViewportDimensions[2]
		{ -8192, 8191 },                                  // viewportBoundsRange[2]
		0,                                                // viewportSubPixelBits
		64,                                               // minMemoryMapAlignment
		vk::MIN_TEXEL_BUFFER_OFFSET_ALIGNMENT,            // minTexelBufferOffsetAlignment
		vk::MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT,          // minUniformBufferOffsetAlignment
		vk::MIN_STORAGE_BUFFER_OFFSET_ALIGNMENT,          // minStorageBufferOffsetAlignment
		sw::MIN_TEXEL_OFFSET,                             // minTexelOffset
		sw::MAX_TEXEL_OFFSET,                             // maxTexelOffset
		sw::MIN_TEXEL_OFFSET,                             // minTexelGatherOffset
		sw::MAX_TEXEL_OFFSET,                             // maxTexelGatherOffset
		-0.5,                                             // minInterpolationOffset
		0.5,                                              // maxInterpolationOffset
		4,                                                // subPixelInterpolationOffsetBits
		4096,                                             // maxFramebufferWidth
		4096,                                             // maxFramebufferHeight
		256,                                              // maxFramebufferLayers
		sampleCounts,                                     // framebufferColorSampleCounts
		sampleCounts,                                     // framebufferDepthSampleCounts
		sampleCounts,                                     // framebufferStencilSampleCounts
		sampleCounts,                                     // framebufferNoAttachmentsSampleCounts
		4,                                                // maxColorAttachments
		sampleCounts,                                     // sampledImageColorSampleCounts
		sampleCounts,                                     // sampledImageIntegerSampleCounts
		sampleCounts,                                     // sampledImageDepthSampleCounts
		sampleCounts,                                     // sampledImageStencilSampleCounts
		sampleCounts,                                     // storageImageSampleCounts
		1,                                                // maxSampleMaskWords
		VK_FALSE,                                         // timestampComputeAndGraphics
		60,                                               // timestampPeriod
		sw::MAX_CLIP_DISTANCES,                           // maxClipDistances
		sw::MAX_CULL_DISTANCES,                           // maxCullDistances
		sw::MAX_CLIP_DISTANCES + sw::MAX_CULL_DISTANCES,  // maxCombinedClipAndCullDistances
		2,                                                // discreteQueuePriorities
		{ 1.0, vk::MAX_POINT_SIZE },                      // pointSizeRange[2]
		{ 1.0, 1.0 },                                     // lineWidthRange[2] (unsupported)
		0.0,                                              // pointSizeGranularity (unsupported)
		0.0,                                              // lineWidthGranularity (unsupported)
		VK_TRUE,                                          // strictLines
		VK_TRUE,                                          // standardSampleLocations
		64,                                               // optimalBufferCopyOffsetAlignment
		64,                                               // optimalBufferCopyRowPitchAlignment
		256,                                              // nonCoherentAtomSize
	};

	return limits;
}

const VkPhysicalDeviceProperties &PhysicalDevice::getProperties() const
{
	auto getProperties = [&]() -> VkPhysicalDeviceProperties {
		VkPhysicalDeviceProperties properties = {
			API_VERSION,
			DRIVER_VERSION,
			VENDOR_ID,
			DEVICE_ID,
			VK_PHYSICAL_DEVICE_TYPE_CPU,  // deviceType
			"",                           // deviceName
			SWIFTSHADER_UUID,             // pipelineCacheUUID
			getLimits(),                  // limits
			{}                            // sparseProperties
		};

		// Append Reactor JIT backend name and version
		snprintf(properties.deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE,
		         "%s (%s)", SWIFTSHADER_DEVICE_NAME, rr::BackendName().c_str());

		return properties;
	};

	static const VkPhysicalDeviceProperties properties = getProperties();
	return properties;
}

void PhysicalDevice::getProperties(VkPhysicalDeviceIDProperties *properties) const
{
	memset(properties->deviceUUID, 0, VK_UUID_SIZE);
	memset(properties->driverUUID, 0, VK_UUID_SIZE);
	memset(properties->deviceLUID, 0, VK_LUID_SIZE);

	memcpy(properties->deviceUUID, SWIFTSHADER_UUID, VK_UUID_SIZE);
	*((uint64_t *)properties->driverUUID) = DRIVER_VERSION;

	properties->deviceNodeMask = 0;
	properties->deviceLUIDValid = VK_FALSE;
}

void PhysicalDevice::getProperties(VkPhysicalDeviceMaintenance3Properties *properties) const
{
	properties->maxMemoryAllocationSize = MAX_MEMORY_ALLOCATION_SIZE;
	properties->maxPerSetDescriptors = 1024;
}

void PhysicalDevice::getProperties(VkPhysicalDeviceMultiviewProperties *properties) const
{
	properties->maxMultiviewViewCount = 6;
	properties->maxMultiviewInstanceIndex = 1u << 27;
}

void PhysicalDevice::getProperties(VkPhysicalDevicePointClippingProperties *properties) const
{
	properties->pointClippingBehavior = VK_POINT_CLIPPING_BEHAVIOR_ALL_CLIP_PLANES;
}

void PhysicalDevice::getProperties(VkPhysicalDeviceProtectedMemoryProperties *properties) const
{
	properties->protectedNoFault = VK_FALSE;
}

void PhysicalDevice::getProperties(VkPhysicalDeviceSubgroupProperties *properties) const
{
	properties->subgroupSize = sw::SIMD::Width;
	properties->supportedStages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;
	properties->supportedOperations =
	    VK_SUBGROUP_FEATURE_BASIC_BIT |
	    VK_SUBGROUP_FEATURE_VOTE_BIT |
	    VK_SUBGROUP_FEATURE_ARITHMETIC_BIT |
	    VK_SUBGROUP_FEATURE_BALLOT_BIT |
	    VK_SUBGROUP_FEATURE_SHUFFLE_BIT |
	    VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT;
	properties->quadOperationsInAllStages = VK_FALSE;
}

void PhysicalDevice::getProperties(const VkExternalMemoryHandleTypeFlagBits *handleType, VkExternalImageFormatProperties *properties) const
{
	setExternalMemoryProperties(*handleType, &properties->externalMemoryProperties);
}

void PhysicalDevice::getProperties(VkSamplerYcbcrConversionImageFormatProperties *properties) const
{
	properties->combinedImageSamplerDescriptorCount = 1;  // Need only one descriptor for YCbCr sampling.
}

#ifdef __ANDROID__
void PhysicalDevice::getProperties(VkPhysicalDevicePresentationPropertiesANDROID *properties) const
{
	properties->sharedImage = VK_FALSE;
}
#endif

void PhysicalDevice::getProperties(const VkPhysicalDeviceExternalBufferInfo *pExternalBufferInfo, VkExternalBufferProperties *pExternalBufferProperties) const
{
	setExternalMemoryProperties(pExternalBufferInfo->handleType, &pExternalBufferProperties->externalMemoryProperties);
}

void PhysicalDevice::getProperties(const VkPhysicalDeviceExternalFenceInfo *pExternalFenceInfo, VkExternalFenceProperties *pExternalFenceProperties) const
{
	pExternalFenceProperties->compatibleHandleTypes = 0;
	pExternalFenceProperties->exportFromImportedHandleTypes = 0;
	pExternalFenceProperties->externalFenceFeatures = 0;
}

void PhysicalDevice::getProperties(const VkPhysicalDeviceExternalSemaphoreInfo *pExternalSemaphoreInfo, VkExternalSemaphoreProperties *pExternalSemaphoreProperties) const
{
#if SWIFTSHADER_EXTERNAL_SEMAPHORE_OPAQUE_FD
	if(pExternalSemaphoreInfo->handleType == VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT)
	{
		pExternalSemaphoreProperties->compatibleHandleTypes = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT;
		pExternalSemaphoreProperties->exportFromImportedHandleTypes = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT;
		pExternalSemaphoreProperties->externalSemaphoreFeatures = VK_EXTERNAL_SEMAPHORE_FEATURE_EXPORTABLE_BIT | VK_EXTERNAL_SEMAPHORE_FEATURE_IMPORTABLE_BIT;
		return;
	}
#endif
#if VK_USE_PLATFORM_FUCHSIA
	if(pExternalSemaphoreInfo->handleType == VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TEMP_ZIRCON_EVENT_BIT_FUCHSIA)
	{
		pExternalSemaphoreProperties->compatibleHandleTypes = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TEMP_ZIRCON_EVENT_BIT_FUCHSIA;
		pExternalSemaphoreProperties->exportFromImportedHandleTypes = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TEMP_ZIRCON_EVENT_BIT_FUCHSIA;
		pExternalSemaphoreProperties->externalSemaphoreFeatures = VK_EXTERNAL_SEMAPHORE_FEATURE_EXPORTABLE_BIT | VK_EXTERNAL_SEMAPHORE_FEATURE_IMPORTABLE_BIT;
		return;
	}
#endif
	pExternalSemaphoreProperties->compatibleHandleTypes = 0;
	pExternalSemaphoreProperties->exportFromImportedHandleTypes = 0;
	pExternalSemaphoreProperties->externalSemaphoreFeatures = 0;
}

void PhysicalDevice::getProperties(VkPhysicalDeviceExternalMemoryHostPropertiesEXT *properties) const
{
	properties->minImportedHostPointerAlignment = REQUIRED_MEMORY_ALIGNMENT;
}

void PhysicalDevice::getProperties(VkPhysicalDeviceDriverPropertiesKHR *properties) const
{
	properties->driverID = VK_DRIVER_ID_GOOGLE_SWIFTSHADER_KHR;
	strcpy(properties->driverName, "SwiftShader driver");
	strcpy(properties->driverInfo, "");
	properties->conformanceVersion = { 1, 1, 3, 3 };
}

void PhysicalDevice::getProperties(VkPhysicalDeviceLineRasterizationPropertiesEXT *properties) const
{
	properties->lineSubPixelPrecisionBits = vk::SUBPIXEL_PRECISION_BITS;
}

void PhysicalDevice::getProperties(VkPhysicalDeviceProvokingVertexPropertiesEXT *properties) const
{
	properties->provokingVertexModePerPipeline = VK_TRUE;
}

bool PhysicalDevice::hasFeatures(const VkPhysicalDeviceFeatures &requestedFeatures) const
{
	const VkPhysicalDeviceFeatures &supportedFeatures = getFeatures();
	const VkBool32 *supportedFeature = reinterpret_cast<const VkBool32 *>(&supportedFeatures);
	const VkBool32 *requestedFeature = reinterpret_cast<const VkBool32 *>(&requestedFeatures);
	constexpr auto featureCount = sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32);

	for(unsigned int i = 0; i < featureCount; i++)
	{
		if((requestedFeature[i] != VK_FALSE) && (supportedFeature[i] == VK_FALSE))
		{
			return false;
		}
	}

	return true;
}

void PhysicalDevice::getFormatProperties(Format format, VkFormatProperties *pFormatProperties) const
{
	pFormatProperties->linearTilingFeatures = 0;   // Unsupported format
	pFormatProperties->optimalTilingFeatures = 0;  // Unsupported format
	pFormatProperties->bufferFeatures = 0;         // Unsupported format

	switch(format)
	{
		// Formats which can be sampled *and* filtered
		case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
		case VK_FORMAT_R5G6B5_UNORM_PACK16:
		case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
		case VK_FORMAT_R8_UNORM:
		case VK_FORMAT_R8_SRGB:
		case VK_FORMAT_R8_SNORM:
		case VK_FORMAT_R8G8_UNORM:
		case VK_FORMAT_R8G8_SRGB:
		case VK_FORMAT_R8G8_SNORM:
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_R8G8B8A8_SNORM:
		case VK_FORMAT_R8G8B8A8_SRGB:
		case VK_FORMAT_B8G8R8A8_UNORM:
		case VK_FORMAT_B8G8R8A8_SRGB:
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		case VK_FORMAT_R16_SFLOAT:
		case VK_FORMAT_R16G16_SFLOAT:
		case VK_FORMAT_R16G16B16A16_SFLOAT:
		case VK_FORMAT_R32_SFLOAT:
		case VK_FORMAT_R32G32_SFLOAT:
		case VK_FORMAT_R32G32B32A32_SFLOAT:
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
		case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
		case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
		case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
		case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
		case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
		case VK_FORMAT_BC2_UNORM_BLOCK:
		case VK_FORMAT_BC2_SRGB_BLOCK:
		case VK_FORMAT_BC3_UNORM_BLOCK:
		case VK_FORMAT_BC3_SRGB_BLOCK:
		case VK_FORMAT_BC4_UNORM_BLOCK:
		case VK_FORMAT_BC4_SNORM_BLOCK:
		case VK_FORMAT_BC5_UNORM_BLOCK:
		case VK_FORMAT_BC5_SNORM_BLOCK:
		case VK_FORMAT_BC6H_UFLOAT_BLOCK:
		case VK_FORMAT_BC6H_SFLOAT_BLOCK:
		case VK_FORMAT_BC7_UNORM_BLOCK:
		case VK_FORMAT_BC7_SRGB_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
		case VK_FORMAT_EAC_R11_UNORM_BLOCK:
		case VK_FORMAT_EAC_R11_SNORM_BLOCK:
		case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
		case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
#ifdef SWIFTSHADER_ENABLE_ASTC
		case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
		case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
		case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
		case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
		case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
		case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
		case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
		case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
		case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
		case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
		case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
		case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
		case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
		case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
		case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
		case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
		case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
		case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
		case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
		case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
		case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
		case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
		case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
		case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
		case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
		case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
		case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
		case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
#endif
		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_D32_SFLOAT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			pFormatProperties->optimalTilingFeatures |=
			    VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
			// [[fallthrough]]

		// Formats which can be sampled, but don't support filtering
		case VK_FORMAT_R8_UINT:
		case VK_FORMAT_R8_SINT:
		case VK_FORMAT_R8G8_UINT:
		case VK_FORMAT_R8G8_SINT:
		case VK_FORMAT_R8G8B8A8_UINT:
		case VK_FORMAT_R8G8B8A8_SINT:
		case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		case VK_FORMAT_A2B10G10R10_UINT_PACK32:
		case VK_FORMAT_A2R10G10B10_UINT_PACK32:
		case VK_FORMAT_R16_UINT:
		case VK_FORMAT_R16_SINT:
		case VK_FORMAT_R16G16_UINT:
		case VK_FORMAT_R16G16_SINT:
		case VK_FORMAT_R16G16B16A16_UINT:
		case VK_FORMAT_R16G16B16A16_SINT:
		case VK_FORMAT_R32_UINT:
		case VK_FORMAT_R32_SINT:
		case VK_FORMAT_R32G32_UINT:
		case VK_FORMAT_R32G32_SINT:
		case VK_FORMAT_R32G32B32A32_UINT:
		case VK_FORMAT_R32G32B32A32_SINT:
		case VK_FORMAT_S8_UINT:
			pFormatProperties->optimalTilingFeatures |=
			    VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
			    VK_FORMAT_FEATURE_BLIT_SRC_BIT |
			    VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
			    VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
			break;

		// YCbCr formats:
		case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
		case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
			pFormatProperties->optimalTilingFeatures |=
			    VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
			    VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
			    VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
			    VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
			    VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
			break;
		default:
			break;
	}

	switch(format)
	{
		case VK_FORMAT_R32_UINT:
		case VK_FORMAT_R32_SINT:
			pFormatProperties->optimalTilingFeatures |=
			    VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
			pFormatProperties->bufferFeatures |=
			    VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
			// [[fallthrough]]
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_R8G8B8A8_SNORM:
		case VK_FORMAT_R8G8B8A8_UINT:
		case VK_FORMAT_R8G8B8A8_SINT:
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		case VK_FORMAT_R16G16B16A16_UINT:
		case VK_FORMAT_R16G16B16A16_SINT:
		case VK_FORMAT_R16G16B16A16_SFLOAT:
		case VK_FORMAT_R32_SFLOAT:
		case VK_FORMAT_R32G32_UINT:
		case VK_FORMAT_R32G32_SINT:
		case VK_FORMAT_R32G32_SFLOAT:
		case VK_FORMAT_R32G32B32A32_UINT:
		case VK_FORMAT_R32G32B32A32_SINT:
		case VK_FORMAT_R32G32B32A32_SFLOAT:
		// shaderStorageImageExtendedFormats
		case VK_FORMAT_R16G16_SFLOAT:
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
		case VK_FORMAT_R16_SFLOAT:
		case VK_FORMAT_R16G16B16A16_UNORM:
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		case VK_FORMAT_R16G16_UNORM:
		case VK_FORMAT_R8G8_UNORM:
		case VK_FORMAT_R16_UNORM:
		case VK_FORMAT_R8_UNORM:
		case VK_FORMAT_R16G16B16A16_SNORM:
		case VK_FORMAT_R16G16_SNORM:
		case VK_FORMAT_R8G8_SNORM:
		case VK_FORMAT_R16_SNORM:
		case VK_FORMAT_R8_SNORM:
		case VK_FORMAT_R16G16_SINT:
		case VK_FORMAT_R8G8_SINT:
		case VK_FORMAT_R16_SINT:
		case VK_FORMAT_R8_SINT:
		case VK_FORMAT_A2B10G10R10_UINT_PACK32:
		case VK_FORMAT_R16G16_UINT:
		case VK_FORMAT_R8G8_UINT:
		case VK_FORMAT_R16_UINT:
		case VK_FORMAT_R8_UINT:
			pFormatProperties->optimalTilingFeatures |=
			    VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
			// [[fallthrough]]
			pFormatProperties->bufferFeatures |=
			    VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
			break;
		default:
			break;
	}

	switch(format)
	{
		case VK_FORMAT_R5G6B5_UNORM_PACK16:
		case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
		case VK_FORMAT_R8_UNORM:
		case VK_FORMAT_R8G8_UNORM:
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_R8G8B8A8_SRGB:
		case VK_FORMAT_B8G8R8A8_UNORM:
		case VK_FORMAT_B8G8R8A8_SRGB:
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		case VK_FORMAT_R16_SFLOAT:
		case VK_FORMAT_R16G16_SFLOAT:
		case VK_FORMAT_R16G16B16A16_SFLOAT:
		case VK_FORMAT_R32_SFLOAT:
		case VK_FORMAT_R32G32_SFLOAT:
		case VK_FORMAT_R32G32B32A32_SFLOAT:
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
		case VK_FORMAT_R8_UINT:
		case VK_FORMAT_R8_SINT:
		case VK_FORMAT_R8G8_UINT:
		case VK_FORMAT_R8G8_SINT:
		case VK_FORMAT_R8G8B8A8_UINT:
		case VK_FORMAT_R8G8B8A8_SINT:
		case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		case VK_FORMAT_A2B10G10R10_UINT_PACK32:
		case VK_FORMAT_A2R10G10B10_UINT_PACK32:
		case VK_FORMAT_R16_UINT:
		case VK_FORMAT_R16_SINT:
		case VK_FORMAT_R16G16_UINT:
		case VK_FORMAT_R16G16_SINT:
		case VK_FORMAT_R16G16B16A16_UINT:
		case VK_FORMAT_R16G16B16A16_SINT:
		case VK_FORMAT_R32_UINT:
		case VK_FORMAT_R32_SINT:
		case VK_FORMAT_R32G32_UINT:
		case VK_FORMAT_R32G32_SINT:
		case VK_FORMAT_R32G32B32A32_UINT:
		case VK_FORMAT_R32G32B32A32_SINT:
			pFormatProperties->optimalTilingFeatures |=
			    VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
			    VK_FORMAT_FEATURE_BLIT_DST_BIT;
			break;
		case VK_FORMAT_S8_UINT:
		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_D32_SFLOAT:          // Note: either VK_FORMAT_D32_SFLOAT or VK_FORMAT_X8_D24_UNORM_PACK32 must be supported
		case VK_FORMAT_D32_SFLOAT_S8_UINT:  // Note: either VK_FORMAT_D24_UNORM_S8_UINT or VK_FORMAT_D32_SFLOAT_S8_UINT must be supported
			pFormatProperties->optimalTilingFeatures |=
			    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
			break;
		default:
			break;
	}

	if(format.supportsColorAttachmentBlend())
	{
		pFormatProperties->optimalTilingFeatures |=
		    VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
	}

	switch(format)
	{
		case VK_FORMAT_R8_UNORM:
		case VK_FORMAT_R8_SNORM:
		case VK_FORMAT_R8_UINT:
		case VK_FORMAT_R8_SINT:
		case VK_FORMAT_R8G8_UNORM:
		case VK_FORMAT_R8G8_SNORM:
		case VK_FORMAT_R8G8_UINT:
		case VK_FORMAT_R8G8_SINT:
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_R8G8B8A8_SNORM:
		case VK_FORMAT_R8G8B8A8_UINT:
		case VK_FORMAT_R8G8B8A8_SINT:
		case VK_FORMAT_B8G8R8A8_UNORM:
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
		case VK_FORMAT_A2R10G10B10_UINT_PACK32:
		case VK_FORMAT_A2R10G10B10_SINT_PACK32:
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
		case VK_FORMAT_A2B10G10R10_UINT_PACK32:
		case VK_FORMAT_A2B10G10R10_SINT_PACK32:
		case VK_FORMAT_R16_UNORM:
		case VK_FORMAT_R16_SNORM:
		case VK_FORMAT_R16_UINT:
		case VK_FORMAT_R16_SINT:
		case VK_FORMAT_R16_SFLOAT:
		case VK_FORMAT_R16G16_UNORM:
		case VK_FORMAT_R16G16_SNORM:
		case VK_FORMAT_R16G16_UINT:
		case VK_FORMAT_R16G16_SINT:
		case VK_FORMAT_R16G16_SFLOAT:
		case VK_FORMAT_R16G16B16A16_UNORM:
		case VK_FORMAT_R16G16B16A16_SNORM:
		case VK_FORMAT_R16G16B16A16_UINT:
		case VK_FORMAT_R16G16B16A16_SINT:
		case VK_FORMAT_R16G16B16A16_SFLOAT:
		case VK_FORMAT_R32_UINT:
		case VK_FORMAT_R32_SINT:
		case VK_FORMAT_R32_SFLOAT:
		case VK_FORMAT_R32G32_UINT:
		case VK_FORMAT_R32G32_SINT:
		case VK_FORMAT_R32G32_SFLOAT:
		case VK_FORMAT_R32G32B32_UINT:
		case VK_FORMAT_R32G32B32_SINT:
		case VK_FORMAT_R32G32B32_SFLOAT:
		case VK_FORMAT_R32G32B32A32_UINT:
		case VK_FORMAT_R32G32B32A32_SINT:
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			pFormatProperties->bufferFeatures |=
			    VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
			break;
		default:
			break;
	}

	switch(format)
	{
		// Vulkan 1.1 mandatory
		case VK_FORMAT_R8_UNORM:
		case VK_FORMAT_R8_SNORM:
		case VK_FORMAT_R8_UINT:
		case VK_FORMAT_R8_SINT:
		case VK_FORMAT_R8G8_UNORM:
		case VK_FORMAT_R8G8_SNORM:
		case VK_FORMAT_R8G8_UINT:
		case VK_FORMAT_R8G8_SINT:
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_R8G8B8A8_SNORM:
		case VK_FORMAT_R8G8B8A8_UINT:
		case VK_FORMAT_R8G8B8A8_SINT:
		case VK_FORMAT_B8G8R8A8_UNORM:
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		case VK_FORMAT_A2B10G10R10_UINT_PACK32:
		case VK_FORMAT_R16_UINT:
		case VK_FORMAT_R16_SINT:
		case VK_FORMAT_R16_SFLOAT:
		case VK_FORMAT_R16G16_UINT:
		case VK_FORMAT_R16G16_SINT:
		case VK_FORMAT_R16G16_SFLOAT:
		case VK_FORMAT_R16G16B16A16_UINT:
		case VK_FORMAT_R16G16B16A16_SINT:
		case VK_FORMAT_R16G16B16A16_SFLOAT:
		case VK_FORMAT_R32_UINT:
		case VK_FORMAT_R32_SINT:
		case VK_FORMAT_R32_SFLOAT:
		case VK_FORMAT_R32G32_UINT:
		case VK_FORMAT_R32G32_SINT:
		case VK_FORMAT_R32G32_SFLOAT:
		case VK_FORMAT_R32G32B32A32_UINT:
		case VK_FORMAT_R32G32B32A32_SINT:
		case VK_FORMAT_R32G32B32A32_SFLOAT:
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
		// Optional
		case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		case VK_FORMAT_A2R10G10B10_UINT_PACK32:
			pFormatProperties->bufferFeatures |=
			    VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
			break;
		default:
			break;
	}

	if(pFormatProperties->optimalTilingFeatures)
	{
		pFormatProperties->linearTilingFeatures = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
		                                          VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
	}
}

void PhysicalDevice::getImageFormatProperties(Format format, VkImageType type, VkImageTiling tiling,
                                              VkImageUsageFlags usage, VkImageCreateFlags flags,
                                              VkImageFormatProperties *pImageFormatProperties) const
{
	pImageFormatProperties->sampleCounts = VK_SAMPLE_COUNT_1_BIT;
	pImageFormatProperties->maxArrayLayers = vk::MAX_IMAGE_ARRAY_LAYERS;
	pImageFormatProperties->maxExtent.depth = 1;

	switch(type)
	{
		case VK_IMAGE_TYPE_1D:
			pImageFormatProperties->maxMipLevels = vk::MAX_IMAGE_LEVELS_1D;
			pImageFormatProperties->maxExtent.width = 1 << (vk::MAX_IMAGE_LEVELS_1D - 1);
			pImageFormatProperties->maxExtent.height = 1;
			break;
		case VK_IMAGE_TYPE_2D:
			if(flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
			{
				pImageFormatProperties->maxMipLevels = vk::MAX_IMAGE_LEVELS_CUBE;
				pImageFormatProperties->maxExtent.width = 1 << (vk::MAX_IMAGE_LEVELS_CUBE - 1);
				pImageFormatProperties->maxExtent.height = 1 << (vk::MAX_IMAGE_LEVELS_CUBE - 1);
			}
			else
			{
				pImageFormatProperties->maxMipLevels = vk::MAX_IMAGE_LEVELS_2D;
				pImageFormatProperties->maxExtent.width = 1 << (vk::MAX_IMAGE_LEVELS_2D - 1);
				pImageFormatProperties->maxExtent.height = 1 << (vk::MAX_IMAGE_LEVELS_2D - 1);

				VkFormatProperties props;
				getFormatProperties(format, &props);
				auto features = tiling == VK_IMAGE_TILING_LINEAR ? props.linearTilingFeatures : props.optimalTilingFeatures;
				if(features & (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
				{
					// Only renderable formats make sense for multisample
					pImageFormatProperties->sampleCounts = getSampleCounts();
				}
			}
			break;
		case VK_IMAGE_TYPE_3D:
			pImageFormatProperties->maxMipLevels = vk::MAX_IMAGE_LEVELS_3D;
			pImageFormatProperties->maxExtent.width = 1 << (vk::MAX_IMAGE_LEVELS_3D - 1);
			pImageFormatProperties->maxExtent.height = 1 << (vk::MAX_IMAGE_LEVELS_3D - 1);
			pImageFormatProperties->maxExtent.depth = 1 << (vk::MAX_IMAGE_LEVELS_3D - 1);
			pImageFormatProperties->maxArrayLayers = 1;  // no 3D + layers
			break;
		default:
			UNREACHABLE("VkImageType: %d", int(type));
			break;
	}

	pImageFormatProperties->maxResourceSize = 1u << 31;  // Minimum value for maxResourceSize

	// "Images created with tiling equal to VK_IMAGE_TILING_LINEAR have further restrictions on their limits and capabilities
	//  compared to images created with tiling equal to VK_IMAGE_TILING_OPTIMAL."
	if(tiling == VK_IMAGE_TILING_LINEAR)
	{
		pImageFormatProperties->maxMipLevels = 1;
		pImageFormatProperties->maxArrayLayers = 1;
		pImageFormatProperties->sampleCounts = VK_SAMPLE_COUNT_1_BIT;
	}

	// "Images created with a format from one of those listed in Formats requiring sampler Y'CbCr conversion for VK_IMAGE_ASPECT_COLOR_BIT image views
	//  have further restrictions on their limits and capabilities compared to images created with other formats."
	if(format.isYcbcrFormat())
	{
		pImageFormatProperties->maxMipLevels = 1;  // TODO(b/151263485): This is relied on by the sampler to disable mipmapping for Y'CbCr image sampling.
		pImageFormatProperties->maxArrayLayers = 1;
		pImageFormatProperties->sampleCounts = VK_SAMPLE_COUNT_1_BIT;
	}
}

uint32_t PhysicalDevice::getQueueFamilyPropertyCount() const
{
	return 1;
}

void PhysicalDevice::getQueueFamilyProperties(uint32_t pQueueFamilyPropertyCount,
                                              VkQueueFamilyProperties *pQueueFamilyProperties) const
{
	for(uint32_t i = 0; i < pQueueFamilyPropertyCount; i++)
	{
		pQueueFamilyProperties[i].minImageTransferGranularity.width = 1;
		pQueueFamilyProperties[i].minImageTransferGranularity.height = 1;
		pQueueFamilyProperties[i].minImageTransferGranularity.depth = 1;
		pQueueFamilyProperties[i].queueCount = 1;
		pQueueFamilyProperties[i].queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
		pQueueFamilyProperties[i].timestampValidBits = 0;  // No support for time stamps
	}
}

void PhysicalDevice::getQueueFamilyProperties(uint32_t pQueueFamilyPropertyCount,
                                              VkQueueFamilyProperties2 *pQueueFamilyProperties) const
{
	for(uint32_t i = 0; i < pQueueFamilyPropertyCount; i++)
	{
		pQueueFamilyProperties[i].queueFamilyProperties.minImageTransferGranularity.width = 1;
		pQueueFamilyProperties[i].queueFamilyProperties.minImageTransferGranularity.height = 1;
		pQueueFamilyProperties[i].queueFamilyProperties.minImageTransferGranularity.depth = 1;
		pQueueFamilyProperties[i].queueFamilyProperties.queueCount = 1;
		pQueueFamilyProperties[i].queueFamilyProperties.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
		pQueueFamilyProperties[i].queueFamilyProperties.timestampValidBits = 0;  // No support for time stamps
	}
}

const VkPhysicalDeviceMemoryProperties &PhysicalDevice::getMemoryProperties() const
{
	static const VkPhysicalDeviceMemoryProperties properties{
		1,  // memoryTypeCount
		{
		    // vk::MEMORY_TYPE_GENERIC_BIT
		    {
		        (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
		         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
		         VK_MEMORY_PROPERTY_HOST_CACHED_BIT),  // propertyFlags
		        0                                      // heapIndex
		    },
		},
		1,  // memoryHeapCount
		{
		    {
		        1ull << 31,                      // size, FIXME(sugoi): This should be configurable based on available RAM
		        VK_MEMORY_HEAP_DEVICE_LOCAL_BIT  // flags
		    },
		}
	};

	return properties;
}

}  // namespace vk
