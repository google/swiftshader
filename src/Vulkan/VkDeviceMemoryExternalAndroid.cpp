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

#	include "VkDestroy.hpp"
#	include "VkDevice.hpp"
#	include "VkFormat.hpp"
#	include "VkObject.hpp"
#	include "VkPhysicalDevice.hpp"

#	include "System/Debug.hpp"
#	include "System/Linux/MemFd.hpp"
#	include <sys/mman.h>

#	include <android/hardware_buffer.h>
#	include <cutils/native_handle.h>
#	include <vndk/hardware_buffer.h>

#	include <cros_gralloc/cros_gralloc_handle.h>
#	include <unistd.h>
#	include <virgl_hw.h>
#	include <virtgpu_drm.h>
#	include <xf86drm.h>

AHardwareBufferExternalMemory::~AHardwareBufferExternalMemory()
{
	// correct deallocation of AHB does not require a pointer or size
	deallocate(nullptr, 0);
}

VkResult AHardwareBufferExternalMemory::allocate(size_t size, void **pBuffer)
{
	if(allocateInfo.importAhb)
	{
		ahb = allocateInfo.ahb;
		AHardwareBuffer_acquire(ahb);
		return allocateAndroidHardwareBuffer(size, pBuffer);
	}
	else
	{
		ASSERT(allocateInfo.exportAhb);

		// Outline ahbDesc
		AHardwareBuffer_Desc ahbDesc;
		if(allocateInfo.imageHandle)
		{
			ahbDesc.format = GetAndroidHardwareBufferDescFormat(VkFormat(allocateInfo.imageHandle->getFormat()));
			VkExtent3D extent = allocateInfo.imageHandle->getMipLevelExtent(VK_IMAGE_ASPECT_COLOR_BIT, 0);
			ahbDesc.width = extent.width;
			ahbDesc.height = extent.height;
			ahbDesc.layers = allocateInfo.imageHandle->getArrayLayers();
			ahbDesc.stride = allocateInfo.imageHandle->rowPitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, 0);

			VkImageCreateFlags createFlags = allocateInfo.imageHandle->getFlags();
			VkImageUsageFlags usageFlags = allocateInfo.imageHandle->getUsage();
			GetAndroidHardwareBufferUsageFromVkUsage(createFlags, usageFlags, ahbDesc.usage);
		}
		else
		{
			ASSERT(allocateInfo.bufferHandle);
			ahbDesc.format = AHARDWAREBUFFER_FORMAT_BLOB;
			ahbDesc.width = uint32_t(allocateInfo.bufferHandle->getSize());
			ahbDesc.height = 1;
			ahbDesc.layers = 1;
			ahbDesc.stride = uint32_t(allocateInfo.bufferHandle->getSize());
			// TODO(b/141698760)
			//   This will be fairly specific, needs fleshing out
			ahbDesc.usage = AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN | AHARDWAREBUFFER_USAGE_CPU_WRITE_OFTEN;
		}

		// create a new ahb from desc
		if(AHardwareBuffer_allocate(&ahbDesc, &ahb) != 0)
		{
			return VK_ERROR_OUT_OF_HOST_MEMORY;
		}

		return allocateAndroidHardwareBuffer(size, pBuffer);
	}
}

VkResult AHardwareBufferExternalMemory::allocateAndroidHardwareBuffer(size_t size, void **pBuffer)
{
	// get native_handle_t from ahb
	const native_handle_t *h = AHardwareBuffer_getNativeHandle(ahb);
	if(h == nullptr)
	{
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}

	// get rendernodeFD and primeHandle from native_handle_t.data
	uint32_t primeHandle;
	createRenderNodeFD();
	VkResult result = getPrimeHandle(h, primeHandle);
	if(result != VK_SUCCESS)
	{
		return result;
	}

	// get memory pointer from store or mmap it
	vk::Device::AHBAddressMap *pDeviceHandleMap = device->getAHBAddressMap();
	void *pAddress = pDeviceHandleMap->query(primeHandle);
	if(pAddress != nullptr)
	{
		*pBuffer = pAddress;
		pDeviceHandleMap->incrementReference(primeHandle);
	}
	else
	{
		// map memory
		void *ptr;
		VkResult result = mapMemory(primeHandle, &ptr);
		if(result != VK_SUCCESS)
		{
			return result;
		}

		// Add primeHandle and ptr to deviceHandleMap
		pDeviceHandleMap->add(primeHandle, ptr);
		*pBuffer = pDeviceHandleMap->query(primeHandle);
	}

	return VK_SUCCESS;
}

void AHardwareBufferExternalMemory::deallocate(void *buffer, size_t size)
{
	if(ahb != nullptr)
	{
		const native_handle_t *h = AHardwareBuffer_getNativeHandle(ahb);
		uint32_t primeHandle;
		VkResult result = getPrimeHandle(h, primeHandle);
		ASSERT(result == VK_SUCCESS);

		// close gpu memory and rendernodeFD
		vk::Device::AHBAddressMap *pDeviceHandleMap = device->getAHBAddressMap();
		if(pDeviceHandleMap->decrementReference(primeHandle) == 0)
		{
			closeMemory(primeHandle);
		}
		close(rendernodeFD);

		AHardwareBuffer_release(ahb);
		ahb = nullptr;
	}
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

uint32_t AHardwareBufferExternalMemory::GetAndroidHardwareBufferDescFormat(VkFormat format)
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

VkFormat AHardwareBufferExternalMemory::GetVkFormat(uint32_t ahbFormat)
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

VkFormatFeatureFlags AHardwareBufferExternalMemory::GetVkFormatFeatures(VkFormat format)
{
	VkFormatProperties formatProperties;
	vk::PhysicalDevice::GetFormatProperties(vk::Format(format), &formatProperties);

	formatProperties.optimalTilingFeatures |= VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;

	// TODO: b/167896057
	//   The correct formatFeatureFlags depends on consumer and format
	//   So this solution is incomplete without more information
	return formatProperties.linearTilingFeatures | formatProperties.optimalTilingFeatures | formatProperties.bufferFeatures;
}

VkResult AHardwareBufferExternalMemory::GetAndroidHardwareBufferFormatProperties(const AHardwareBuffer_Desc &ahbDesc, VkAndroidHardwareBufferFormatPropertiesANDROID *pFormat)
{

	pFormat->sType = VK_STRUCTURE_TYPE_ANDROID_HARDWARE_BUFFER_FORMAT_PROPERTIES_ANDROID;
	pFormat->pNext = nullptr;

	pFormat->format = GetVkFormat(ahbDesc.format);
	pFormat->externalFormat = ahbDesc.format;
	pFormat->formatFeatures = GetVkFormatFeatures(pFormat->format);

	pFormat->samplerYcbcrConversionComponents = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	pFormat->suggestedYcbcrModel = VK_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY;
	pFormat->suggestedYcbcrRange = VK_SAMPLER_YCBCR_RANGE_ITU_FULL;
	pFormat->suggestedXChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
	pFormat->suggestedYChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;

	return VK_SUCCESS;
}

VkResult AHardwareBufferExternalMemory::GetAndroidHardwareBufferProperties(VkDevice &device, const struct AHardwareBuffer *buffer, VkAndroidHardwareBufferPropertiesANDROID *pProperties)
{
	AHardwareBuffer_Desc ahbDesc;
	AHardwareBuffer_describe(buffer, &ahbDesc);

	GetAndroidHardwareBufferFormatProperties(ahbDesc, (VkAndroidHardwareBufferFormatPropertiesANDROID *)pProperties->pNext);

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
		info.format = GetVkFormat(ahbDesc.format);
		info.extent.width = ahbDesc.width;
		info.extent.height = ahbDesc.height;
		info.extent.depth = 1;
		info.mipLevels = 1;
		info.arrayLayers = 1;
		info.samples = VK_SAMPLE_COUNT_1_BIT;
		info.tiling = VK_IMAGE_TILING_OPTIMAL;
		info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		VkImage Image;
		VkResult result = vk::Image::Create(vk::DEVICE_MEMORY, &info, &Image, vk::Cast(device));

		pProperties->allocationSize = vk::Cast(Image)->getMemoryRequirements().size;
		vk::destroy(Image, vk::DEVICE_MEMORY);
	}

	return VK_SUCCESS;
}

VkResult AHardwareBufferExternalMemory::GetAndroidHardwareBufferUsageFromVkUsage(const VkImageCreateFlags createFlags, const VkImageUsageFlags usageFlags, uint64_t &ahbDescUsage)
{
	if(usageFlags & VK_IMAGE_USAGE_SAMPLED_BIT)
		ahbDescUsage |= AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE;
	if(usageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)
		ahbDescUsage |= AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE;
	if(usageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		ahbDescUsage |= AHARDWAREBUFFER_USAGE_GPU_COLOR_OUTPUT;

	if(createFlags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
		ahbDescUsage |= AHARDWAREBUFFER_USAGE_GPU_CUBE_MAP;
	if(createFlags & VK_IMAGE_CREATE_PROTECTED_BIT)
		ahbDescUsage |= AHARDWAREBUFFER_USAGE_PROTECTED_CONTENT;

	// No usage bits set - set at least one GPU usage
	if(ahbDescUsage == 0)
		ahbDescUsage = AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE;

	return VK_SUCCESS;
}

// Call into the native gralloc implementation to request a handle for the
// rendernodeFD
VkResult AHardwareBufferExternalMemory::getPrimeHandle(const native_handle_t *h, uint32_t &primeHandle)
{
	cros_gralloc_handle const *crosHandle = reinterpret_cast<cros_gralloc_handle const *>(h);
	int ret = drmPrimeFDToHandle(rendernodeFD, crosHandle->fds[0], &primeHandle);
	if(ret != 0)
	{
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}

	return VK_SUCCESS;
}

// Create a rendernodeFD
void AHardwareBufferExternalMemory::createRenderNodeFD()
{
	rendernodeFD = drmOpenRender(128);
}

// using a primeHandle associated with a specific rendernodeFD, map a new block
// of memory
VkResult AHardwareBufferExternalMemory::mapMemory(uint32_t &primeHandle, void **ptr)
{
	drm_virtgpu_map map;
	memset(&map, 0, sizeof(drm_virtgpu_map));
	map.handle = primeHandle;
	int ret = drmIoctl(rendernodeFD, DRM_IOCTL_VIRTGPU_MAP, &map);
	if(ret != 0)
	{
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}

	// mmap it
	*ptr = static_cast<unsigned char *>(
	    mmap64(nullptr, 4096, PROT_WRITE, MAP_SHARED, rendernodeFD, map.offset));
	if(ptr == MAP_FAILED)
	{
		return VK_ERROR_MEMORY_MAP_FAILED;
	}

	return VK_SUCCESS;
}

// Close out the memory block associated with rendernodeFD
VkResult AHardwareBufferExternalMemory::closeMemory(uint32_t &primeHandle)
{
	struct drm_gem_close gem_close;
	memset(&gem_close, 0x0, sizeof(gem_close));
	gem_close.handle = primeHandle;
	int ret = drmIoctl(rendernodeFD, DRM_IOCTL_GEM_CLOSE, &gem_close);
	if(ret != 0)
	{
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}

	return VK_SUCCESS;
}

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

				if(exportInfo->handleTypes != VK_EXTERNAL_MEMORY_HANDLE_TYPE_ANDROID_HARDWARE_BUFFER_BIT_ANDROID)
				{
					UNSUPPORTED("VkExportMemoryAllocateInfo::handleTypes %d", int(exportInfo->handleTypes));
				}
				exportAhb = true;
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

#endif  // SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
