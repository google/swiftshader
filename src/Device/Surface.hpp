// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
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

#ifndef sw_Surface_hpp
#define sw_Surface_hpp

#include "Color.hpp"
#include "Device/Config.hpp"
#include "System/Resource.hpp"
#include <vulkan/vulkan.h>

namespace sw
{
	class Resource;

	template <typename T> struct RectT
	{
		RectT() {}
		RectT(T x0i, T y0i, T x1i, T y1i) : x0(x0i), y0(y0i), x1(x1i), y1(y1i) {}

		void clip(T minX, T minY, T maxX, T maxY)
		{
			x0 = clamp(x0, minX, maxX);
			y0 = clamp(y0, minY, maxY);
			x1 = clamp(x1, minX, maxX);
			y1 = clamp(y1, minY, maxY);
		}

		T width() const  { return x1 - x0; }
		T height() const { return y1 - y0; }

		T x0;   // Inclusive
		T y0;   // Inclusive
		T x1;   // Exclusive
		T y1;   // Exclusive
	};

	typedef RectT<int> Rect;
	typedef RectT<float> RectF;

	template<typename T> struct SliceRectT : public RectT<T>
	{
		SliceRectT() : slice(0) {}
		SliceRectT(const RectT<T>& rect) : RectT<T>(rect), slice(0) {}
		SliceRectT(const RectT<T>& rect, int s) : RectT<T>(rect), slice(s) {}
		SliceRectT(T x0, T y0, T x1, T y1, int s) : RectT<T>(x0, y0, x1, y1), slice(s) {}
		int slice;
	};

	typedef SliceRectT<int> SliceRect;
	typedef SliceRectT<float> SliceRectF;

	enum Format : unsigned char
	{
		FORMAT_NULL = VK_FORMAT_UNDEFINED,
		// VK_FORMAT_R4G4_UNORM_PACK8,
		// VK_FORMAT_R4G4B4A4_UNORM_PACK16,
		FORMAT_R4G4B4A4 = VK_FORMAT_B4G4R4A4_UNORM_PACK16, // Mandatory
		FORMAT_R5G6B5 = VK_FORMAT_R5G6B5_UNORM_PACK16, // Mandatory
		// VK_FORMAT_B5G6R5_UNORM_PACK16,
		FORMAT_R5G5B5A1 = VK_FORMAT_R5G5B5A1_UNORM_PACK16,
		// VK_FORMAT_B5G5R5A1_UNORM_PACK16,
		FORMAT_A1R5G5B5 = VK_FORMAT_A1R5G5B5_UNORM_PACK16, // Mandatory
		FORMAT_R8 = VK_FORMAT_R8_UNORM, // Mandatory
		FORMAT_R8_SNORM = VK_FORMAT_R8_SNORM, // Mandatory
		// VK_FORMAT_R8_USCALED,
		// VK_FORMAT_R8_SSCALED,
		FORMAT_R8UI = VK_FORMAT_R8_UINT, // Mandatory
		FORMAT_R8I = VK_FORMAT_R8_SINT, // Mandatory
		// VK_FORMAT_R8_SRGB,
		FORMAT_G8R8 = VK_FORMAT_R8G8_UNORM, // Mandatory
		FORMAT_G8R8_SNORM = VK_FORMAT_R8G8_SNORM, // Mandatory
		// VK_FORMAT_R8G8_USCALED,
		// VK_FORMAT_R8G8_SSCALED,
		FORMAT_G8R8UI = VK_FORMAT_R8G8_UINT, // Mandatory
		FORMAT_G8R8I = VK_FORMAT_R8G8_SINT, // Mandatory
		// VK_FORMAT_R8G8_SRGB,
		// VK_FORMAT_R8G8B8_UNORM,
		// VK_FORMAT_R8G8B8_SNORM,
		// VK_FORMAT_R8G8B8_USCALED,
		// VK_FORMAT_R8G8B8_SSCALED,
		// VK_FORMAT_R8G8B8_UINT,
		// VK_FORMAT_R8G8B8_SINT,
		// VK_FORMAT_R8G8B8_SRGB,
		// VK_FORMAT_B8G8R8_UNORM,
		// VK_FORMAT_B8G8R8_SNORM,
		// VK_FORMAT_B8G8R8_USCALED,
		// VK_FORMAT_B8G8R8_SSCALED,
		// VK_FORMAT_B8G8R8_UINT,
		// VK_FORMAT_B8G8R8_SINT,
		// VK_FORMAT_B8G8R8_SRGB,
		FORMAT_A8B8G8R8 = VK_FORMAT_R8G8B8A8_UNORM, // Mandatory
		FORMAT_A8B8G8R8_SNORM = VK_FORMAT_R8G8B8A8_SNORM, // Mandatory
		// VK_FORMAT_R8G8B8A8_USCALED,
		// VK_FORMAT_R8G8B8A8_SSCALED,
		FORMAT_A8B8G8R8UI = VK_FORMAT_R8G8B8A8_UINT, // Mandatory
		FORMAT_A8B8G8R8I = VK_FORMAT_R8G8B8A8_SINT, // Mandatory
		FORMAT_SRGB8_A8 = VK_FORMAT_R8G8B8A8_SRGB, // Mandatory
		FORMAT_A8R8G8B8 = VK_FORMAT_B8G8R8A8_UNORM, // Mandatory
		// VK_FORMAT_B8G8R8A8_SNORM,
		// VK_FORMAT_B8G8R8A8_USCALED,
		// VK_FORMAT_B8G8R8A8_SSCALED,
		// VK_FORMAT_B8G8R8A8_UINT,
		// VK_FORMAT_B8G8R8A8_SINT,
		// VK_FORMAT_B8G8R8A8_SRGB, // TODO: Mandatory
		// VK_FORMAT_A8B8G8R8_UNORM_PACK32, // TODO: Mandatory
		// VK_FORMAT_A8B8G8R8_SNORM_PACK32, // TODO: Mandatory
		// VK_FORMAT_A8B8G8R8_USCALED_PACK32,
		// VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
		// VK_FORMAT_A8B8G8R8_UINT_PACK32, // TODO: Mandatory
		// VK_FORMAT_A8B8G8R8_SINT_PACK32, // TODO: Mandatory
		// VK_FORMAT_A8B8G8R8_SRGB_PACK32, // TODO: Mandatory
		FORMAT_A2R10G10B10 = VK_FORMAT_A2R10G10B10_UNORM_PACK32,
		// VK_FORMAT_A2R10G10B10_SNORM_PACK32,
		// VK_FORMAT_A2R10G10B10_USCALED_PACK32,
		// VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
		// VK_FORMAT_A2R10G10B10_UINT_PACK32,
		// VK_FORMAT_A2R10G10B10_SINT_PACK32,
		FORMAT_A2B10G10R10 = VK_FORMAT_A2B10G10R10_UNORM_PACK32, // Mandatory
		// VK_FORMAT_A2B10G10R10_SNORM_PACK32,
		// VK_FORMAT_A2B10G10R10_USCALED_PACK32,
		// VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
		FORMAT_A2B10G10R10UI = VK_FORMAT_A2B10G10R10_UINT_PACK32, // Mandatory
		// VK_FORMAT_A2B10G10R10_SINT_PACK32,
		// VK_FORMAT_R16_UNORM, // Mandatory (Vertex buffer only)
		// VK_FORMAT_R16_SNORM, // Mandatory (Vertex buffer only)
		// VK_FORMAT_R16_USCALED,
		// VK_FORMAT_R16_SSCALED,
		FORMAT_R16UI = VK_FORMAT_R16_UINT, // Mandatory
		FORMAT_R16I = VK_FORMAT_R16_SINT, // Mandatory
		FORMAT_R16F = VK_FORMAT_R16_SFLOAT, // Mandatory
		FORMAT_G16R16 = VK_FORMAT_R16G16_UNORM, // Mandatory (Vertex buffer only)
		// VK_FORMAT_R16G16_SNORM, // Mandatory (Vertex buffer only)
		// VK_FORMAT_R16G16_USCALED,
		// VK_FORMAT_R16G16_SSCALED,
		FORMAT_G16R16UI = VK_FORMAT_R16G16_UINT, // Mandatory
		FORMAT_G16R16I = VK_FORMAT_R16G16_SINT, // Mandatory
		FORMAT_G16R16F = VK_FORMAT_R16G16_SFLOAT, // Mandatory
		// VK_FORMAT_R16G16B16_UNORM,
		// VK_FORMAT_R16G16B16_SNORM,
		// VK_FORMAT_R16G16B16_USCALED,
		// VK_FORMAT_R16G16B16_SSCALED,
		// VK_FORMAT_R16G16B16_UINT,
		// VK_FORMAT_R16G16B16_SINT,
		// VK_FORMAT_R16G16B16_SFLOAT,
		FORMAT_A16B16G16R16 = VK_FORMAT_R16G16B16A16_UNORM, // Mandatory (Vertex buffer only)
		// VK_FORMAT_R16G16B16A16_SNORM, // Mandatory (Vertex buffer only)
		// VK_FORMAT_R16G16B16A16_USCALED,
		// VK_FORMAT_R16G16B16A16_SSCALED,
		FORMAT_A16B16G16R16UI = VK_FORMAT_R16G16B16A16_UINT, // Mandatory
		FORMAT_A16B16G16R16I = VK_FORMAT_R16G16B16A16_SINT, // Mandatory
		FORMAT_A16B16G16R16F = VK_FORMAT_R16G16B16A16_SFLOAT, // Mandatory
		FORMAT_R32UI = VK_FORMAT_R32_UINT, // Mandatory
		FORMAT_R32I = VK_FORMAT_R32_SINT, // Mandatory
		FORMAT_R32F = VK_FORMAT_R32_SFLOAT, // Mandatory
		FORMAT_G32R32UI = VK_FORMAT_R32G32_UINT, // Mandatory
		FORMAT_G32R32I = VK_FORMAT_R32G32_SINT, // Mandatory
		FORMAT_G32R32F = VK_FORMAT_R32G32_SFLOAT, // Mandatory
		// VK_FORMAT_R32G32B32_UINT, // Mandatory (Vertex buffer only)
		// VK_FORMAT_R32G32B32_SINT, // Mandatory (Vertex buffer only)
		// VK_FORMAT_R32G32B32_SFLOAT, // Mandatory (Vertex buffer only)
		FORMAT_A32B32G32R32UI = VK_FORMAT_R32G32B32A32_UINT, // Mandatory
		FORMAT_A32B32G32R32I = VK_FORMAT_R32G32B32A32_SINT, // Mandatory
		FORMAT_A32B32G32R32F = VK_FORMAT_R32G32B32A32_SFLOAT, // Mandatory
		// VK_FORMAT_R64_UINT,
		// VK_FORMAT_R64_SINT,
		// VK_FORMAT_R64_SFLOAT,
		// VK_FORMAT_R64G64_UINT,
		// VK_FORMAT_R64G64_SINT,
		// VK_FORMAT_R64G64_SFLOAT,
		// VK_FORMAT_R64G64B64_UINT,
		// VK_FORMAT_R64G64B64_SINT,
		// VK_FORMAT_R64G64B64_SFLOAT,
		// VK_FORMAT_R64G64B64A64_UINT,
		// VK_FORMAT_R64G64B64A64_SINT,
		// VK_FORMAT_R64G64B64A64_SFLOAT,
		// VK_FORMAT_B10G11R11_UFLOAT_PACK32, // TODO: Mandatory
		// VK_FORMAT_E5B9G9R9_UFLOAT_PACK32, // TODO: Mandatory
		FORMAT_D16 = VK_FORMAT_D16_UNORM, // Mandatory
		FORMAT_D24X8 = VK_FORMAT_X8_D24_UNORM_PACK32, // Feature specific
		FORMAT_D32F = VK_FORMAT_D32_SFLOAT, // Mandatory
		FORMAT_S8 = VK_FORMAT_S8_UINT,
		// VK_FORMAT_D16_UNORM_S8_UINT,
		FORMAT_D24S8 = VK_FORMAT_D24_UNORM_S8_UINT, // Feature specific
		FORMAT_D32FS8 = VK_FORMAT_D32_SFLOAT_S8_UINT, // Feature specific
		// VK_FORMAT_BC1_RGB_UNORM_BLOCK, // Feature specific
		// VK_FORMAT_BC1_RGB_SRGB_BLOCK, // Feature specific
		// VK_FORMAT_BC1_RGBA_UNORM_BLOCK, // Feature specific
		// VK_FORMAT_BC1_RGBA_SRGB_BLOCK, // Feature specific
		// VK_FORMAT_BC2_UNORM_BLOCK, // Feature specific
		// VK_FORMAT_BC2_SRGB_BLOCK, // Feature specific
		// VK_FORMAT_BC3_UNORM_BLOCK, // Feature specific
		// VK_FORMAT_BC3_SRGB_BLOCK, // Feature specific
		// VK_FORMAT_BC4_UNORM_BLOCK, // Feature specific
		// VK_FORMAT_BC4_SNORM_BLOCK, // Feature specific
		// VK_FORMAT_BC5_UNORM_BLOCK, // Feature specific
		// VK_FORMAT_BC5_SNORM_BLOCK, // Feature specific
		// VK_FORMAT_BC6H_UFLOAT_BLOCK, // Feature specific
		// VK_FORMAT_BC6H_SFLOAT_BLOCK, // Feature specific
		// VK_FORMAT_BC7_UNORM_BLOCK, // Feature specific
		// VK_FORMAT_BC7_SRGB_BLOCK, // Feature specific
		FORMAT_RGB8_ETC2 = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_ETC2 = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK, // Feature specific
		FORMAT_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 = VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 = VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK, // Feature specific
		FORMAT_RGBA8_ETC2_EAC = VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_ALPHA8_ETC2_EAC = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK, // Feature specific
		FORMAT_R11_EAC = VK_FORMAT_EAC_R11_UNORM_BLOCK, // Feature specific
		FORMAT_SIGNED_R11_EAC = VK_FORMAT_EAC_R11_SNORM_BLOCK, // Feature specific
		FORMAT_RG11_EAC = VK_FORMAT_EAC_R11G11_UNORM_BLOCK, // Feature specific
		FORMAT_SIGNED_RG11_EAC = VK_FORMAT_EAC_R11G11_SNORM_BLOCK, // Feature specific
		FORMAT_RGBA_ASTC_4x4_KHR = VK_FORMAT_ASTC_4x4_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_ALPHA8_ASTC_4x4_KHR = VK_FORMAT_ASTC_4x4_SRGB_BLOCK, // Feature specific
		FORMAT_RGBA_ASTC_5x4_KHR = VK_FORMAT_ASTC_5x4_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_ALPHA8_ASTC_5x4_KHR = VK_FORMAT_ASTC_5x4_SRGB_BLOCK, // Feature specific
		FORMAT_RGBA_ASTC_5x5_KHR = VK_FORMAT_ASTC_5x5_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_ALPHA8_ASTC_5x5_KHR = VK_FORMAT_ASTC_5x5_SRGB_BLOCK, // Feature specific
		FORMAT_RGBA_ASTC_6x5_KHR = VK_FORMAT_ASTC_6x5_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_ALPHA8_ASTC_6x5_KHR = VK_FORMAT_ASTC_6x5_SRGB_BLOCK, // Feature specific
		FORMAT_RGBA_ASTC_6x6_KHR = VK_FORMAT_ASTC_6x6_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_ALPHA8_ASTC_6x6_KHR = VK_FORMAT_ASTC_6x6_SRGB_BLOCK, // Feature specific
		FORMAT_RGBA_ASTC_8x5_KHR = VK_FORMAT_ASTC_8x5_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_ALPHA8_ASTC_8x5_KHR = VK_FORMAT_ASTC_8x5_SRGB_BLOCK, // Feature specific
		FORMAT_RGBA_ASTC_8x6_KHR = VK_FORMAT_ASTC_8x6_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_ALPHA8_ASTC_8x6_KHR = VK_FORMAT_ASTC_8x6_SRGB_BLOCK, // Feature specific
		FORMAT_RGBA_ASTC_8x8_KHR = VK_FORMAT_ASTC_8x8_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_ALPHA8_ASTC_8x8_KHR = VK_FORMAT_ASTC_8x8_SRGB_BLOCK, // Feature specific
		FORMAT_RGBA_ASTC_10x5_KHR = VK_FORMAT_ASTC_10x5_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_ALPHA8_ASTC_10x5_KHR = VK_FORMAT_ASTC_10x5_SRGB_BLOCK, // Feature specific
		FORMAT_RGBA_ASTC_10x6_KHR = VK_FORMAT_ASTC_10x6_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_ALPHA8_ASTC_10x6_KHR = VK_FORMAT_ASTC_10x6_SRGB_BLOCK, // Feature specific
		FORMAT_RGBA_ASTC_10x8_KHR = VK_FORMAT_ASTC_10x8_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_ALPHA8_ASTC_10x8_KHR = VK_FORMAT_ASTC_10x8_SRGB_BLOCK, // Feature specific
		FORMAT_RGBA_ASTC_10x10_KHR = VK_FORMAT_ASTC_10x10_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_ALPHA8_ASTC_10x10_KHR = VK_FORMAT_ASTC_10x10_SRGB_BLOCK, // Feature specific
		FORMAT_RGBA_ASTC_12x10_KHR = VK_FORMAT_ASTC_12x10_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_ALPHA8_ASTC_12x10_KHR = VK_FORMAT_ASTC_12x10_SRGB_BLOCK, // Feature specific
		FORMAT_RGBA_ASTC_12x12_KHR = VK_FORMAT_ASTC_12x12_UNORM_BLOCK, // Feature specific
		FORMAT_SRGB8_ALPHA8_ASTC_12x12_KHR = VK_FORMAT_ASTC_12x12_SRGB_BLOCK, // Feature specific

		// EXTENSION FORMATS:
		// IMPORTANT: if any of these formats are added, this enum needs
		// to go from "unsigned char" to "unsigned int" and FORMAT_LAST
		// below needs to be updated to take extensions into account.

		// VK_FORMAT_G8B8G8R8_422_UNORM,
		// VK_FORMAT_B8G8R8G8_422_UNORM,
		// VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM, // TODO: Mandatory
		// VK_FORMAT_G8_B8R8_2PLANE_420_UNORM, // TODO: Mandatory
		// VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM,
		// VK_FORMAT_G8_B8R8_2PLANE_422_UNORM,
		// VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM,
		// VK_FORMAT_R10X6_UNORM_PACK16,
		// VK_FORMAT_R10X6G10X6_UNORM_2PACK16,
		// VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16,
		// VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
		// VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
		// VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
		// VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
		// VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
		// VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
		// VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
		// VK_FORMAT_R12X4_UNORM_PACK16,
		// VK_FORMAT_R12X4G12X4_UNORM_2PACK16,
		// VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16,
		// VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
		// VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
		// VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
		// VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
		// VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
		// VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
		// VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
		// VK_FORMAT_G16B16G16R16_422_UNORM,
		// VK_FORMAT_B16G16R16G16_422_UNORM,
		// VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM,
		// VK_FORMAT_G16_B16R16_2PLANE_420_UNORM,
		// VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM,
		// VK_FORMAT_G16_B16R16_2PLANE_422_UNORM,
		// VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM,
		// VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG,
		// VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG,
		// VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG,
		// VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG,
		// VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG,
		// VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG,
		// VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG,
		// VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG,

		// YUV formats
		FORMAT_YV12_BT601 = VK_FORMAT_END_RANGE + 1, // TODO: VK_FORMAT_G8_B8R8_2PLANE_420_UNORM + VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_601
		FORMAT_YV12_BT709,                           // TODO: VK_FORMAT_G8_B8R8_2PLANE_420_UNORM + VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_709
		FORMAT_YV12_JFIF,                            // TODO: VK_FORMAT_G8_B8R8_2PLANE_420_UNORM + VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020

		FORMAT_LAST = FORMAT_YV12_JFIF
	};

	enum Lock
	{
		LOCK_UNLOCKED,
		LOCK_READONLY,
		LOCK_WRITEONLY,
		LOCK_READWRITE,
		LOCK_DISCARD,
		LOCK_UPDATE   // Write access which doesn't dirty the buffer, because it's being updated with the sibling's data.
	};

	class Surface
	{
	private:
		struct Buffer
		{
			friend Surface;

		private:
			void write(int x, int y, int z, const Color<float> &color);
			void write(int x, int y, const Color<float> &color);
			void write(void *element, const Color<float> &color);
			Color<float> read(int x, int y, int z) const;
			Color<float> read(int x, int y) const;
			Color<float> read(void *element) const;
			Color<float> sample(float x, float y, float z) const;
			Color<float> sample(float x, float y, int layer) const;

			void *lockRect(int x, int y, int z, Lock lock);
			void unlockRect();

			void *buffer;
			int width;
			int height;
			int depth;
			short border;
			short samples;

			int bytes;
			int pitchB;
			int pitchP;
			int sliceB;
			int sliceP;

			Format format;
			AtomicInt lock;

			bool dirty;   // Sibling internal/external buffer doesn't match.
		};

	protected:
		Surface(int width, int height, int depth, Format format, void *pixels, int pitch, int slice);
		Surface(Resource *texture, int width, int height, int depth, int border, int samples, Format format, bool lockable, bool renderTarget, int pitchP = 0);

	public:
		static Surface *create(int width, int height, int depth, Format format, void *pixels, int pitch, int slice);
		static Surface *create(Resource *texture, int width, int height, int depth, int border, int samples, Format format, bool lockable, bool renderTarget, int pitchP = 0);

		virtual ~Surface() = 0;

		inline void *lock(int x, int y, int z, Lock lock, Accessor client, bool internal = false);
		inline void unlock(bool internal = false);
		inline int getWidth() const;
		inline int getHeight() const;
		inline int getDepth() const;
		inline int getBorder() const;
		inline Format getFormat(bool internal = false) const;
		inline int getPitchB(bool internal = false) const;
		inline int getPitchP(bool internal = false) const;
		inline int getSliceB(bool internal = false) const;
		inline int getSliceP(bool internal = false) const;

		void *lockExternal(int x, int y, int z, Lock lock, Accessor client);
		void unlockExternal();
		inline Format getExternalFormat() const;
		inline int getExternalPitchB() const;
		inline int getExternalPitchP() const;
		inline int getExternalSliceB() const;
		inline int getExternalSliceP() const;

		virtual void *lockInternal(int x, int y, int z, Lock lock, Accessor client) = 0;
		virtual void unlockInternal() = 0;
		inline Format getInternalFormat() const;
		inline int getInternalPitchB() const;
		inline int getInternalPitchP() const;
		inline int getInternalSliceB() const;
		inline int getInternalSliceP() const;

		void *lockStencil(int x, int y, int front, Accessor client);
		void unlockStencil();
		inline Format getStencilFormat() const;
		inline int getStencilPitchB() const;
		inline int getStencilSliceB() const;

		void sync();                      // Wait for lock(s) to be released.
		virtual bool requiresSync() const { return false; }
		inline bool isUnlocked() const;   // Only reliable after sync().

		inline int getSamples() const;
		inline int getMultiSampleCount() const;
		inline int getSuperSampleCount() const;

		bool isEntire(const Rect& rect) const;
		Rect getRect() const;
		void clearDepth(float depth, int x0, int y0, int width, int height);
		void clearStencil(unsigned char stencil, unsigned char mask, int x0, int y0, int width, int height);
		void fill(const Color<float> &color, int x0, int y0, int width, int height);

		Color<float> readExternal(int x, int y, int z) const;
		Color<float> readExternal(int x, int y) const;
		Color<float> sampleExternal(float x, float y, float z) const;
		Color<float> sampleExternal(float x, float y) const;
		void writeExternal(int x, int y, int z, const Color<float> &color);
		void writeExternal(int x, int y, const Color<float> &color);

		void copyInternal(const Surface* src, int x, int y, float srcX, float srcY, bool filter);
		void copyInternal(const Surface* src, int x, int y, int z, float srcX, float srcY, float srcZ, bool filter);

		enum Edge { TOP, BOTTOM, RIGHT, LEFT };
		void copyCubeEdge(Edge dstEdge, Surface *src, Edge srcEdge);
		void computeCubeCorner(int x0, int y0, int x1, int y1);

		bool hasStencil() const;
		bool hasDepth() const;
		bool isRenderTarget() const;

		bool hasDirtyContents() const;
		void markContentsClean();
		inline bool isExternalDirty() const;
		Resource *getResource();

		static int bytes(Format format);
		static int pitchB(int width, int border, Format format, bool target);
		static int pitchP(int width, int border, Format format, bool target);
		static int sliceB(int width, int height, int border, Format format, bool target);
		static int sliceP(int width, int height, int border, Format format, bool target);
		static size_t size(int width, int height, int depth, int border, int samples, Format format);

		static bool isStencil(Format format);
		static bool isDepth(Format format);
		static bool hasQuadLayout(Format format);

		static bool isFloatFormat(Format format);
		static bool isUnsignedComponent(Format format, int component);
		static bool isSRGBreadable(Format format);
		static bool isSRGBwritable(Format format);
		static bool isSRGBformat(Format format);
		static bool isCompressed(Format format);
		static bool isSignedNonNormalizedInteger(Format format);
		static bool isUnsignedNonNormalizedInteger(Format format);
		static bool isNonNormalizedInteger(Format format);
		static bool isNormalizedInteger(Format format);
		static int componentCount(Format format);

	private:
		sw::Resource *resource;

		typedef unsigned char byte;
		typedef unsigned short word;
		typedef unsigned int dword;
		typedef uint64_t qword;

		struct DXT1
		{
			word c0;
			word c1;
			dword lut;
		};

		struct DXT3
		{
			qword a;

			word c0;
			word c1;
			dword lut;
		};

		struct DXT5
		{
			union
			{
				struct
				{
					byte a0;
					byte a1;
				};

				qword alut;   // Skip first 16 bit
			};

			word c0;
			word c1;
			dword clut;
		};

		struct ATI2
		{
			union
			{
				struct
				{
					byte y0;
					byte y1;
				};

				qword ylut;   // Skip first 16 bit
			};

			union
			{
				struct
				{
					byte x0;
					byte x1;
				};

				qword xlut;   // Skip first 16 bit
			};
		};

		struct ATI1
		{
			union
			{
				struct
				{
					byte r0;
					byte r1;
				};

				qword rlut;   // Skip first 16 bit
			};
		};

		static void decodeDXT1(Buffer &internal, Buffer &external);
		static void decodeDXT3(Buffer &internal, Buffer &external);
		static void decodeDXT5(Buffer &internal, Buffer &external);
		static void decodeATI1(Buffer &internal, Buffer &external);
		static void decodeATI2(Buffer &internal, Buffer &external);
		static void decodeEAC(Buffer &internal, Buffer &external, int nbChannels, bool isSigned);
		static void decodeETC2(Buffer &internal, Buffer &external, int nbAlphaBits, bool isSRGB);
		static void decodeASTC(Buffer &internal, Buffer &external, int xSize, int ySize, int zSize, bool isSRGB);

		static void update(Buffer &destination, Buffer &source);
		static void genericUpdate(Buffer &destination, Buffer &source);
		static void *allocateBuffer(int width, int height, int depth, int border, int samples, Format format);
		static void memfill4(void *buffer, int pattern, int bytes);

		bool identicalBuffers() const;
		Format selectInternalFormat(Format format) const;

		void resolve();

		Buffer external;
		Buffer internal;
		Buffer stencil;

		const bool lockable;
		const bool renderTarget;

		bool dirtyContents;   // Sibling surfaces need updating (mipmaps / cube borders).

		bool hasParent;
		bool ownExternal;
	};
}

#undef min
#undef max

namespace sw
{
	void *Surface::lock(int x, int y, int z, Lock lock, Accessor client, bool internal)
	{
		return internal ? lockInternal(x, y, z, lock, client) : lockExternal(x, y, z, lock, client);
	}

	void Surface::unlock(bool internal)
	{
		return internal ? unlockInternal() : unlockExternal();
	}

	int Surface::getWidth() const
	{
		return external.width;
	}

	int Surface::getHeight() const
	{
		return external.height;
	}

	int Surface::getDepth() const
	{
		return external.depth;
	}

	int Surface::getBorder() const
	{
		return internal.border;
	}

	Format Surface::getFormat(bool internal) const
	{
		return internal ? getInternalFormat() : getExternalFormat();
	}

	int Surface::getPitchB(bool internal) const
	{
		return internal ? getInternalPitchB() : getExternalPitchB();
	}

	int Surface::getPitchP(bool internal) const
	{
		return internal ? getInternalPitchP() : getExternalPitchP();
	}

	int Surface::getSliceB(bool internal) const
	{
		return internal ? getInternalSliceB() : getExternalSliceB();
	}

	int Surface::getSliceP(bool internal) const
	{
		return internal ? getInternalSliceP() : getExternalSliceP();
	}

	Format Surface::getExternalFormat() const
	{
		return external.format;
	}

	int Surface::getExternalPitchB() const
	{
		return external.pitchB;
	}

	int Surface::getExternalPitchP() const
	{
		return external.pitchP;
	}

	int Surface::getExternalSliceB() const
	{
		return external.sliceB;
	}

	int Surface::getExternalSliceP() const
	{
		return external.sliceP;
	}

	Format Surface::getInternalFormat() const
	{
		return internal.format;
	}

	int Surface::getInternalPitchB() const
	{
		return internal.pitchB;
	}

	int Surface::getInternalPitchP() const
	{
		return internal.pitchP;
	}

	int Surface::getInternalSliceB() const
	{
		return internal.sliceB;
	}

	int Surface::getInternalSliceP() const
	{
		return internal.sliceP;
	}

	Format Surface::getStencilFormat() const
	{
		return stencil.format;
	}

	int Surface::getStencilPitchB() const
	{
		return stencil.pitchB;
	}

	int Surface::getStencilSliceB() const
	{
		return stencil.sliceB;
	}

	int Surface::getSamples() const
	{
		return internal.samples;
	}

	int Surface::getMultiSampleCount() const
	{
		return sw::min((int)internal.samples, 4);
	}

	int Surface::getSuperSampleCount() const
	{
		return internal.samples > 4 ? internal.samples / 4 : 1;
	}

	bool Surface::isUnlocked() const
	{
		return external.lock == LOCK_UNLOCKED &&
		       internal.lock == LOCK_UNLOCKED &&
		       stencil.lock == LOCK_UNLOCKED;
	}

	bool Surface::isExternalDirty() const
	{
		return external.buffer && external.buffer != internal.buffer && external.dirty;
	}
}

#endif   // sw_Surface_hpp
