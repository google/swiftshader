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

#ifndef VK_CONFIG_HPP_
#define VK_CONFIG_HPP_

#include "Version.h"

#include <Vulkan/VulkanPlatform.h>

namespace vk {

// Note: Constant array initialization requires a string literal.
//       constexpr char* or char[] does not work for that purpose.
#define SWIFTSHADER_DEVICE_NAME "SwiftShader Device" // Max length: VK_MAX_PHYSICAL_DEVICE_NAME_SIZE
#define SWIFTSHADER_UUID "SwiftShaderUUID" // Max length: VK_UUID_SIZE (16)

enum
{
	API_VERSION = VK_API_VERSION_1_1,
	DRIVER_VERSION = VK_MAKE_VERSION(MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION),
	VENDOR_ID = 0x1AE0, // Google, Inc.: https://pcisig.com/google-inc-1
	DEVICE_ID = 0xC0DE, // SwiftShader (placeholder)
};

enum
{
	// Alignment of all Vulkan objects, pools, device memory, images, buffers, descriptors.
	REQUIRED_MEMORY_ALIGNMENT = 16,  // 16 bytes for 128-bit vector types.

	MIN_TEXEL_BUFFER_OFFSET_ALIGNMENT = 256,
	MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT = 256,
	MIN_STORAGE_BUFFER_OFFSET_ALIGNMENT = 256,

	MEMORY_TYPE_GENERIC_BIT = 0x1, // Generic system memory.
};

enum
{
	MAX_IMAGE_LEVELS_1D = 14,
	MAX_IMAGE_LEVELS_2D = 14,
	MAX_IMAGE_LEVELS_3D = 11,
	MAX_IMAGE_LEVELS_CUBE = 14,
	MAX_IMAGE_ARRAY_LAYERS = 2048,
	MAX_SAMPLER_LOD_BIAS = 15,
};

enum
{
	MAX_BOUND_DESCRIPTOR_SETS = 4,
	MAX_VERTEX_INPUT_BINDINGS = 16,
	MAX_PUSH_CONSTANT_SIZE = 128,
};

enum
{
	MAX_DESCRIPTOR_SET_UNIFORM_BUFFERS_DYNAMIC = 8,
	MAX_DESCRIPTOR_SET_STORAGE_BUFFERS_DYNAMIC = 4,
	MAX_DESCRIPTOR_SET_COMBINED_BUFFERS_DYNAMIC =
			MAX_DESCRIPTOR_SET_UNIFORM_BUFFERS_DYNAMIC +
			MAX_DESCRIPTOR_SET_STORAGE_BUFFERS_DYNAMIC,
};

enum
{
	MAX_POINT_SIZE = 1023,
};

constexpr int SUBPIXEL_PRECISION_BITS = 4;
constexpr float SUBPIXEL_PRECISION_FACTOR = static_cast<float>(1 << SUBPIXEL_PRECISION_BITS);
constexpr int SUBPIXEL_PRECISION_MASK = 0xFFFFFFFF >> (32 - SUBPIXEL_PRECISION_BITS);

}  // namespace vk

#if defined(__linux__) || defined(__ANDROID__)
#define SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD        1
#define SWIFTSHADER_EXTERNAL_SEMAPHORE_OPAQUE_FD     1
#endif

constexpr VkDeviceSize MAX_MEMORY_ALLOCATION_SIZE = 0x40000000ull;  // 0x40000000 = 1 GiB

// Memory offset calculations in 32-bit SIMD elements limit us to addressing at most 4 GiB.
// Signed arithmetic further restricts it to 2 GiB.
static_assert(MAX_MEMORY_ALLOCATION_SIZE <= 0x80000000ull, "maxMemoryAllocationSize must not exceed 2 GiB");

#endif // VK_CONFIG_HPP_
