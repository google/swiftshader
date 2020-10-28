// Copyright 2020 The SwiftShader Authors. All Rights Reserved.
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

#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER

#	include "VkDeviceMemoryExternalAndroid.hpp"

#	include "System/Debug.hpp"
#	include "VkDestroy.hpp"
#	include "VkFormat.hpp"
#	include "VkObject.hpp"
#	include "VkPhysicalDevice.hpp"
#	include "VkStringify.hpp"

#	include <android/hardware_buffer.h>
#	include <vndk/hardware_buffer.h>

namespace {

int GetBytesFromAHBFormat(uint32_t ahbFormat)
{
	switch(ahbFormat)
	{
		case AHARDWAREBUFFER_FORMAT_D16_UNORM:
			return 2;
		case AHARDWAREBUFFER_FORMAT_D24_UNORM:
			return 3;
		case AHARDWAREBUFFER_FORMAT_D32_FLOAT:
			return 4;
		case AHARDWAREBUFFER_FORMAT_R10G10B10A2_UNORM:
			return 4;
		case AHARDWAREBUFFER_FORMAT_R16G16B16A16_FLOAT:
			return 8;
		case AHARDWAREBUFFER_FORMAT_R5G6B5_UNORM:
			return 2;
		case AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM:
			return 4;
		case AHARDWAREBUFFER_FORMAT_R8G8B8X8_UNORM:
			return 4;
		case AHARDWAREBUFFER_FORMAT_S8_UINT:
			return 1;
		default:
			// TODO(b/165302991)
			// - AHARDWAREBUFFER_FORMAT_BLOB
			// - AHARDWAREBUFFER_FORMAT_D24_UNORM_S8_UINT
			// - AHARDWAREBUFFER_FORMAT_D32_FLOAT_S8_UINT
			// - AHARDWAREBUFFER_FORMAT_R8G8B8_UNORM
			UNSUPPORTED("Requested bytes() for unsupported format %d", int(ahbFormat));
			return -1;
	}
}

uint32_t GetAHBFormatFromVkFormat(VkFormat format)
{
	switch(format)
	{
		case VK_FORMAT_D16_UNORM:
			return AHARDWAREBUFFER_FORMAT_D16_UNORM;
		case VK_FORMAT_X8_D24_UNORM_PACK32:
			UNSUPPORTED("AHardwareBufferExternalMemory::VkFormat VK_FORMAT_X8_D24_UNORM_PACK32");
			return AHARDWAREBUFFER_FORMAT_D24_UNORM;
		case VK_FORMAT_D24_UNORM_S8_UINT:
			UNSUPPORTED("AHardwareBufferExternalMemory::VkFormat VK_FORMAT_D24_UNORM_S8_UINT");
			return AHARDWAREBUFFER_FORMAT_D24_UNORM_S8_UINT;
		case VK_FORMAT_D32_SFLOAT:
			return AHARDWAREBUFFER_FORMAT_D32_FLOAT;
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return AHARDWAREBUFFER_FORMAT_D32_FLOAT_S8_UINT;
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
			return AHARDWAREBUFFER_FORMAT_R10G10B10A2_UNORM;
		case VK_FORMAT_R16G16B16A16_SFLOAT:
			return AHARDWAREBUFFER_FORMAT_R16G16B16A16_FLOAT;
		case VK_FORMAT_R5G6B5_UNORM_PACK16:
			return AHARDWAREBUFFER_FORMAT_R5G6B5_UNORM;
		case VK_FORMAT_R8G8B8A8_UNORM:
			return AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM;
		case VK_FORMAT_R8G8B8_UNORM:
			return AHARDWAREBUFFER_FORMAT_R8G8B8_UNORM;
		case VK_FORMAT_S8_UINT:
			return AHARDWAREBUFFER_FORMAT_S8_UINT;
		case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
			return AHARDWAREBUFFER_FORMAT_Y8Cb8Cr8_420;
		default:
			UNSUPPORTED("AHardwareBufferExternalMemory::VkFormat %d", int(format));
			return 0;
	}
}

VkFormat GetVkFormatFromAHBFormat(uint32_t ahbFormat)
{
	switch(ahbFormat)
	{
		case AHARDWAREBUFFER_FORMAT_BLOB:
			return VK_FORMAT_UNDEFINED;
		case AHARDWAREBUFFER_FORMAT_D16_UNORM:
			return VK_FORMAT_D16_UNORM;
		case AHARDWAREBUFFER_FORMAT_D24_UNORM:
			UNSUPPORTED("AHardwareBufferExternalMemory::AndroidHardwareBuffer_Format AHARDWAREBUFFER_FORMAT_D24_UNORM");
			return VK_FORMAT_X8_D24_UNORM_PACK32;
		case AHARDWAREBUFFER_FORMAT_D24_UNORM_S8_UINT:
			UNSUPPORTED("AHardwareBufferExternalMemory::AndroidHardwareBuffer_Format AHARDWAREBUFFER_FORMAT_D24_UNORM_S8_UINT");
			return VK_FORMAT_X8_D24_UNORM_PACK32;
		case AHARDWAREBUFFER_FORMAT_D32_FLOAT:
			return VK_FORMAT_D32_SFLOAT;
		case AHARDWAREBUFFER_FORMAT_D32_FLOAT_S8_UINT:
			return VK_FORMAT_D32_SFLOAT_S8_UINT;
		case AHARDWAREBUFFER_FORMAT_R10G10B10A2_UNORM:
			return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
		case AHARDWAREBUFFER_FORMAT_R16G16B16A16_FLOAT:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		case AHARDWAREBUFFER_FORMAT_R5G6B5_UNORM:
			return VK_FORMAT_R5G6B5_UNORM_PACK16;
		case AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case AHARDWAREBUFFER_FORMAT_R8G8B8X8_UNORM:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case AHARDWAREBUFFER_FORMAT_R8G8B8_UNORM:
			return VK_FORMAT_R8G8B8_UNORM;
		case AHARDWAREBUFFER_FORMAT_S8_UINT:
			return VK_FORMAT_S8_UINT;
		case AHARDWAREBUFFER_FORMAT_Y8Cb8Cr8_420:
			return VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM;
		default:
			UNSUPPORTED("AHardwareBufferExternalMemory::AHardwareBuffer_Format %d", int(ahbFormat));
			return VK_FORMAT_UNDEFINED;
	}
}

uint64_t GetAHBLockUsageFromVkImageUsageFlags(VkImageUsageFlags flags)
{
	uint64_t usage = 0;

	if(flags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT ||
	   flags & VK_IMAGE_USAGE_SAMPLED_BIT ||
	   flags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
	{
		usage |= AHARDWAREBUFFER_USAGE_CPU_READ_MASK;
	}

	if(flags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ||
	   flags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	{
		usage |= AHARDWAREBUFFER_USAGE_CPU_WRITE_MASK;
	}

	return usage;
}

uint64_t GetAHBLockUsageFromVkBufferUsageFlags(VkBufferUsageFlags flags)
{
	uint64_t usage = 0;

	if(flags & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
	{
		usage |= AHARDWAREBUFFER_USAGE_CPU_READ_MASK;
	}

	if(flags & VK_BUFFER_USAGE_TRANSFER_DST_BIT)
	{
		usage |= AHARDWAREBUFFER_USAGE_CPU_WRITE_MASK;
	}

	return usage;
}

uint64_t GetAHBUsageFromVkImageFlags(VkImageCreateFlags createFlags, VkImageUsageFlags usageFlags)
{
	uint64_t ahbUsage = 0;

	if(usageFlags & VK_IMAGE_USAGE_SAMPLED_BIT)
	{
		ahbUsage |= AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE | AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN;
	}
	if(usageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)
	{
		ahbUsage |= AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE | AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN;
	}
	if(usageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
	{
		ahbUsage |= AHARDWAREBUFFER_USAGE_GPU_COLOR_OUTPUT | AHARDWAREBUFFER_USAGE_CPU_WRITE_OFTEN;
	}

	if(createFlags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
	{
		ahbUsage |= AHARDWAREBUFFER_USAGE_GPU_CUBE_MAP;
	}
	if(createFlags & VK_IMAGE_CREATE_PROTECTED_BIT)
	{
		ahbUsage |= AHARDWAREBUFFER_USAGE_PROTECTED_CONTENT;
	}

	// No usage bits set - set at least one GPU usage
	if(ahbUsage == 0)
	{
		ahbUsage = AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN |
		           AHARDWAREBUFFER_USAGE_CPU_WRITE_OFTEN;
	}

	return ahbUsage;
}

uint64_t GetAHBUsageFromVkBufferFlags(VkBufferCreateFlags /*createFlags*/, VkBufferUsageFlags /*usageFlags*/)
{
	uint64_t ahbUsage = 0;

	// TODO(b/141698760): needs fleshing out.
	ahbUsage = AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN | AHARDWAREBUFFER_USAGE_CPU_WRITE_OFTEN;

	return ahbUsage;
}

VkFormatFeatureFlags GetVkFormatFeaturesFromAHBFormat(uint32_t ahbFormat)
{
	VkFormatFeatureFlags features = 0;

	VkFormat format = GetVkFormatFromAHBFormat(ahbFormat);
	VkFormatProperties formatProperties;
	vk::PhysicalDevice::GetFormatProperties(vk::Format(format), &formatProperties);

	formatProperties.optimalTilingFeatures |= VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;

	// TODO: b/167896057
	//   The correct formatFeatureFlags depends on consumer and format
	//   So this solution is incomplete without more information
	features |= formatProperties.linearTilingFeatures |
	            formatProperties.optimalTilingFeatures |
	            formatProperties.bufferFeatures;

	return features;
}

}  // namespace

AHardwareBufferExternalMemory::AllocateInfo::AllocateInfo(const VkMemoryAllocateInfo *pAllocateInfo)
{
	const auto *createInfo = reinterpret_cast<const VkBaseInStructure *>(pAllocateInfo->pNext);
	while(createInfo)
	{
		switch(createInfo->sType)
		{
			case VK_STRUCTURE_TYPE_IMPORT_ANDROID_HARDWARE_BUFFER_INFO_ANDROID:
			{
				const auto *importInfo = reinterpret_cast<const VkImportAndroidHardwareBufferInfoANDROID *>(createInfo);
				importAhb = true;
				ahb = importInfo->buffer;
			}
			break;
			case VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO:
			{
				const auto *exportInfo = reinterpret_cast<const VkExportMemoryAllocateInfo *>(createInfo);

				if(exportInfo->handleTypes == VK_EXTERNAL_MEMORY_HANDLE_TYPE_ANDROID_HARDWARE_BUFFER_BIT_ANDROID)
				{
					exportAhb = true;
				}
				else
				{
					UNSUPPORTED("VkExportMemoryAllocateInfo::handleTypes %d", int(exportInfo->handleTypes));
				}
			}
			break;
			case VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO:
			{
				// AHB requires dedicated allocation -- for images, the gralloc gets to decide the image layout,
				// not us.
				const auto *dedicatedAllocateInfo = reinterpret_cast<const VkMemoryDedicatedAllocateInfo *>(createInfo);
				imageHandle = vk::Cast(dedicatedAllocateInfo->image);
				bufferHandle = vk::Cast(dedicatedAllocateInfo->buffer);
			}
			break;

			default:
				WARN("VkMemoryAllocateInfo->pNext sType = %s", vk::Stringify(createInfo->sType).c_str());
		}
		createInfo = createInfo->pNext;
	}
}

AHardwareBufferExternalMemory::AHardwareBufferExternalMemory(const VkMemoryAllocateInfo *pAllocateInfo)
    : allocateInfo(pAllocateInfo)
{
}

AHardwareBufferExternalMemory::~AHardwareBufferExternalMemory()
{
	// correct deallocation of AHB does not require a pointer or size
	deallocate(nullptr, 0);
}

// VkAllocateMemory
VkResult AHardwareBufferExternalMemory::allocate(size_t /*size*/, void **pBuffer)
{
	if(allocateInfo.importAhb)
	{
		return importAndroidHardwareBuffer(allocateInfo.ahb, pBuffer);
	}
	else
	{
		ASSERT(allocateInfo.exportAhb);
		return allocateAndroidHardwareBuffer(pBuffer);
	}
}

void AHardwareBufferExternalMemory::deallocate(void *buffer, size_t size)
{
	if(ahb != nullptr)
	{
		unlockAndroidHardwareBuffer();

		AHardwareBuffer_release(ahb);
		ahb = nullptr;
	}
}

VkResult AHardwareBufferExternalMemory::importAndroidHardwareBuffer(struct AHardwareBuffer *buffer, void **pBuffer)
{
	ahb = buffer;

	AHardwareBuffer_acquire(ahb);
	AHardwareBuffer_describe(ahb, &ahbDesc);

	return lockAndroidHardwareBuffer(pBuffer);
}

VkResult AHardwareBufferExternalMemory::allocateAndroidHardwareBuffer(void **pBuffer)
{
	if(allocateInfo.imageHandle)
	{
		vk::Image *image = allocateInfo.imageHandle;
		ASSERT(image != nullptr);
		ASSERT(image->getArrayLayers() == 1);

		VkExtent3D extent = image->getExtent();

		ahbDesc.width = extent.width;
		ahbDesc.height = extent.height;
		ahbDesc.layers = image->getArrayLayers();
		ahbDesc.format = GetAHBFormatFromVkFormat(image->getFormat());
		ahbDesc.usage = GetAHBUsageFromVkImageFlags(image->getFlags(), image->getUsage());
	}
	else
	{
		vk::Buffer *buffer = allocateInfo.bufferHandle;
		ASSERT(buffer != nullptr);

		ahbDesc.width = static_cast<uint32_t>(buffer->getSize());
		ahbDesc.height = 1;
		ahbDesc.layers = 1;
		ahbDesc.format = AHARDWAREBUFFER_FORMAT_BLOB;
		ahbDesc.usage = GetAHBUsageFromVkBufferFlags(buffer->getFlags(), buffer->getUsage());
	}

	int ret = AHardwareBuffer_allocate(&ahbDesc, &ahb);
	if(ret != 0)
	{
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	AHardwareBuffer_describe(ahb, &ahbDesc);

	return lockAndroidHardwareBuffer(pBuffer);
}

VkResult AHardwareBufferExternalMemory::lockAndroidHardwareBuffer(void **pBuffer)
{
	uint64_t usage = 0;
	if(allocateInfo.imageHandle)
	{
		usage = GetAHBLockUsageFromVkImageUsageFlags(allocateInfo.imageHandle->getUsage());
	}
	else
	{
		usage = GetAHBLockUsageFromVkBufferUsageFlags(allocateInfo.bufferHandle->getUsage());
	}

	// Empty fence, lock immedietly.
	int32_t fence = -1;

	// Empty rect, lock entire buffer.
	ARect *rect = nullptr;

	int ret = AHardwareBuffer_lock(ahb, usage, fence, rect, pBuffer);
	if(ret != 0)
	{
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	return VK_SUCCESS;
}

VkResult AHardwareBufferExternalMemory::unlockAndroidHardwareBuffer()
{
	int ret = AHardwareBuffer_unlock(ahb, /*fence=*/nullptr);
	if(ret != 0)
	{
		return VK_ERROR_UNKNOWN;
	}

	return VK_SUCCESS;
}

VkResult AHardwareBufferExternalMemory::exportAndroidHardwareBuffer(struct AHardwareBuffer **pAhb) const
{
	// Each call to vkGetMemoryAndroidHardwareBufferANDROID *must* return an Android hardware buffer with a new reference
	// acquired in addition to the reference held by the VkDeviceMemory. To avoid leaking resources, the application *must*
	// release the reference by calling AHardwareBuffer_release when it is no longer needed.
	AHardwareBuffer_acquire(ahb);
	*pAhb = ahb;
	return VK_SUCCESS;
}

VkResult AHardwareBufferExternalMemory::GetAndroidHardwareBufferFormatProperties(const AHardwareBuffer_Desc &ahbDesc, VkAndroidHardwareBufferFormatPropertiesANDROID *pFormat)
{
	pFormat->sType = VK_STRUCTURE_TYPE_ANDROID_HARDWARE_BUFFER_FORMAT_PROPERTIES_ANDROID;
	pFormat->pNext = nullptr;
	pFormat->format = GetVkFormatFromAHBFormat(ahbDesc.format);
	pFormat->externalFormat = ahbDesc.format;
	pFormat->formatFeatures = GetVkFormatFeaturesFromAHBFormat(ahbDesc.format);
	pFormat->samplerYcbcrConversionComponents = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	pFormat->suggestedYcbcrModel = VK_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY;
	pFormat->suggestedYcbcrRange = VK_SAMPLER_YCBCR_RANGE_ITU_FULL;
	pFormat->suggestedXChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
	pFormat->suggestedYChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;

	return VK_SUCCESS;
}

VkResult AHardwareBufferExternalMemory::GetAndroidHardwareBufferProperties(VkDevice &device, const struct AHardwareBuffer *buffer, VkAndroidHardwareBufferPropertiesANDROID *pProperties)
{
	VkResult result = VK_SUCCESS;

	AHardwareBuffer_Desc ahbDesc;
	AHardwareBuffer_describe(buffer, &ahbDesc);

	if(pProperties->pNext != nullptr)
	{
		result = GetAndroidHardwareBufferFormatProperties(ahbDesc, (VkAndroidHardwareBufferFormatPropertiesANDROID *)pProperties->pNext);
		if(result != VK_SUCCESS)
		{
			return result;
		}
	}

	const VkPhysicalDeviceMemoryProperties phyDeviceMemProps = vk::PhysicalDevice::GetMemoryProperties();
	pProperties->memoryTypeBits = phyDeviceMemProps.memoryTypes[0].propertyFlags;

	if(ahbDesc.format == AHARDWAREBUFFER_FORMAT_BLOB)
	{
		pProperties->allocationSize = ahbDesc.width;
	}
	else
	{
		VkImageCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.imageType = VK_IMAGE_TYPE_2D;
		info.format = GetVkFormatFromAHBFormat(ahbDesc.format);
		info.extent.width = ahbDesc.width;
		info.extent.height = ahbDesc.height;
		info.extent.depth = 1;
		info.mipLevels = 1;
		info.arrayLayers = 1;
		info.samples = VK_SAMPLE_COUNT_1_BIT;
		info.tiling = VK_IMAGE_TILING_OPTIMAL;
		info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		VkImage Image;

		result = vk::Image::Create(vk::DEVICE_MEMORY, &info, &Image, vk::Cast(device));
		if(result != VK_SUCCESS)
		{
			return result;
		}

		pProperties->allocationSize = vk::Cast(Image)->getMemoryRequirements().size;
		vk::destroy(Image, vk::DEVICE_MEMORY);
	}

	return result;
}

int AHardwareBufferExternalMemory::externalImageRowPitchBytes() const
{
	return GetBytesFromAHBFormat(ahbDesc.format) * ahbDesc.stride;
}

#endif  // SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
