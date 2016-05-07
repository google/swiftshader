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
#include "Main/Config.hpp"
#include "Common/Resource.hpp"

namespace sw
{
	class Resource;

	struct Rect
	{
		Rect() {}
		Rect(int x0i, int y0i, int x1i, int y1i) : x0(x0i), y0(y0i), x1(x1i), y1(y1i) {}

		void clip(int minX, int minY, int maxX, int maxY);

		int width() const  { return x1 - x0; }
		int height() const { return y1 - y0; }

		int x0;   // Inclusive
		int y0;   // Inclusive
		int x1;   // Exclusive
		int y1;   // Exclusive
	};

	struct SliceRect : public Rect
	{
		SliceRect() : slice(0) {}
		SliceRect(const Rect& rect) : Rect(rect), slice(0) {}
		SliceRect(const Rect& rect, int s) : Rect(rect), slice(s) {}
		SliceRect(int x0, int y0, int x1, int y1, int s) : Rect(x0, y0, x1, y1), slice(s) {}
		int slice;
	};

	enum Format : unsigned char
	{
		FORMAT_NULL,

		FORMAT_A8,
		FORMAT_R8I,
		FORMAT_R8UI,
		FORMAT_R8I_SNORM,
		FORMAT_R8, // UI_SNORM
		FORMAT_R16I,
		FORMAT_R16UI,
		FORMAT_R32I,
		FORMAT_R32UI,
		FORMAT_R3G3B2,
		FORMAT_A8R3G3B2,
		FORMAT_X4R4G4B4,
		FORMAT_A4R4G4B4,
		FORMAT_R4G4B4A4,
		FORMAT_R5G6B5,
		FORMAT_R8G8B8,
		FORMAT_B8G8R8,
		FORMAT_X8R8G8B8,
		FORMAT_A8R8G8B8,
		FORMAT_X8B8G8R8I,
		FORMAT_X8B8G8R8UI,
		FORMAT_X8B8G8R8I_SNORM,
		FORMAT_X8B8G8R8, // UI_SNORM
		FORMAT_A8B8G8R8I,
		FORMAT_A8B8G8R8UI,
		FORMAT_A8B8G8R8I_SNORM,
		FORMAT_A8B8G8R8, // UI_SNORM
		FORMAT_SRGB8_X8,
		FORMAT_SRGB8_A8,
		FORMAT_X1R5G5B5,
		FORMAT_A1R5G5B5,
		FORMAT_R5G5B5A1,
		FORMAT_G8R8I,
		FORMAT_G8R8UI,
		FORMAT_G8R8I_SNORM,
		FORMAT_G8R8, // UI_SNORM
		FORMAT_G16R16, // D3D format
		FORMAT_G16R16I,
		FORMAT_G16R16UI,
		FORMAT_G32R32I,
		FORMAT_G32R32UI,
		FORMAT_A2R10G10B10,
		FORMAT_A2B10G10R10,
		FORMAT_A16B16G16R16, // D3D format
		FORMAT_X16B16G16R16I,
		FORMAT_X16B16G16R16UI,
		FORMAT_A16B16G16R16I,
		FORMAT_A16B16G16R16UI,
		FORMAT_X32B32G32R32I,
		FORMAT_X32B32G32R32UI,
		FORMAT_A32B32G32R32I,
		FORMAT_A32B32G32R32UI,
		// Paletted formats
		FORMAT_P8,
		FORMAT_A8P8,
		// Compressed formats
		FORMAT_DXT1,
		FORMAT_DXT3,
		FORMAT_DXT5,
		FORMAT_ATI1,
		FORMAT_ATI2,
		FORMAT_ETC1,
		FORMAT_R11_EAC,
		FORMAT_SIGNED_R11_EAC,
		FORMAT_RG11_EAC,
		FORMAT_SIGNED_RG11_EAC,
		FORMAT_RGB8_ETC2,
		FORMAT_SRGB8_ETC2,
		FORMAT_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
		FORMAT_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
		FORMAT_RGBA8_ETC2_EAC,
		FORMAT_SRGB8_ALPHA8_ETC2_EAC,
		FORMAT_RGBA_ASTC_4x4_KHR,
		FORMAT_RGBA_ASTC_5x4_KHR,
		FORMAT_RGBA_ASTC_5x5_KHR,
		FORMAT_RGBA_ASTC_6x5_KHR,
		FORMAT_RGBA_ASTC_6x6_KHR,
		FORMAT_RGBA_ASTC_8x5_KHR,
		FORMAT_RGBA_ASTC_8x6_KHR,
		FORMAT_RGBA_ASTC_8x8_KHR,
		FORMAT_RGBA_ASTC_10x5_KHR,
		FORMAT_RGBA_ASTC_10x6_KHR,
		FORMAT_RGBA_ASTC_10x8_KHR,
		FORMAT_RGBA_ASTC_10x10_KHR,
		FORMAT_RGBA_ASTC_12x10_KHR,
		FORMAT_RGBA_ASTC_12x12_KHR,
		FORMAT_SRGB8_ALPHA8_ASTC_4x4_KHR,
		FORMAT_SRGB8_ALPHA8_ASTC_5x4_KHR,
		FORMAT_SRGB8_ALPHA8_ASTC_5x5_KHR,
		FORMAT_SRGB8_ALPHA8_ASTC_6x5_KHR,
		FORMAT_SRGB8_ALPHA8_ASTC_6x6_KHR,
		FORMAT_SRGB8_ALPHA8_ASTC_8x5_KHR,
		FORMAT_SRGB8_ALPHA8_ASTC_8x6_KHR,
		FORMAT_SRGB8_ALPHA8_ASTC_8x8_KHR,
		FORMAT_SRGB8_ALPHA8_ASTC_10x5_KHR,
		FORMAT_SRGB8_ALPHA8_ASTC_10x6_KHR,
		FORMAT_SRGB8_ALPHA8_ASTC_10x8_KHR,
		FORMAT_SRGB8_ALPHA8_ASTC_10x10_KHR,
		FORMAT_SRGB8_ALPHA8_ASTC_12x10_KHR,
		FORMAT_SRGB8_ALPHA8_ASTC_12x12_KHR,
		// Floating-point formats
		FORMAT_A16F,
		FORMAT_R16F,
		FORMAT_G16R16F,
		FORMAT_B16G16R16F,
		FORMAT_A16B16G16R16F,
		FORMAT_A32F,
		FORMAT_R32F,
		FORMAT_G32R32F,
		FORMAT_B32G32R32F,
		FORMAT_X32B32G32R32F,
		FORMAT_A32B32G32R32F,
		// Bump map formats
		FORMAT_V8U8,
		FORMAT_L6V5U5,
		FORMAT_Q8W8V8U8,
		FORMAT_X8L8V8U8,
		FORMAT_A2W10V10U10,
		FORMAT_V16U16,
		FORMAT_A16W16V16U16,
		FORMAT_Q16W16V16U16,
		// Luminance formats
		FORMAT_L8,
		FORMAT_A4L4,
		FORMAT_L16,
		FORMAT_A8L8,
		FORMAT_L16F,
		FORMAT_A16L16F,
		FORMAT_L32F,
		FORMAT_A32L32F,
		// Depth/stencil formats
		FORMAT_D16,
		FORMAT_D32,
		FORMAT_D24X8,
		FORMAT_D24S8,
		FORMAT_D24FS8,
		FORMAT_D32F,                 // Quad layout
		FORMAT_D32F_COMPLEMENTARY,   // Quad layout, 1 - z
		FORMAT_D32F_LOCKABLE,        // Linear layout
		FORMAT_D32FS8_TEXTURE,       // Linear layout, no PCF
		FORMAT_D32FS8_SHADOW,        // Linear layout, PCF
		FORMAT_DF24S8,
		FORMAT_DF16S8,
		FORMAT_INTZ,
		FORMAT_S8,
		// Quad layout framebuffer
		FORMAT_X8G8R8B8Q,
		FORMAT_A8G8R8B8Q,
		// YUV formats
		FORMAT_YV12_BT601,
		FORMAT_YV12_BT709,
		FORMAT_YV12_JFIF,    // Full-swing BT.601

		FORMAT_LAST = FORMAT_YV12_JFIF
	};

	enum Lock
	{
		LOCK_UNLOCKED,
		LOCK_READONLY,
		LOCK_WRITEONLY,
		LOCK_READWRITE,
		LOCK_DISCARD
	};

	class Surface
	{
	private:
		struct Buffer
		{
		public:
			void write(int x, int y, int z, const Color<float> &color);
			void write(int x, int y, const Color<float> &color);
			void write(void *element, const Color<float> &color);
			Color<float> read(int x, int y, int z) const;
			Color<float> read(int x, int y) const;
			Color<float> read(void *element) const;
			Color<float> sample(float x, float y, float z) const;
			Color<float> sample(float x, float y) const;

			void *lockRect(int x, int y, int z, Lock lock);
			void unlockRect();

			void *buffer;
			int width;
			int height;
			int depth;
			int bytes;
			int pitchB;
			int pitchP;
			int sliceB;
			int sliceP;
			Format format;
			Lock lock;

			bool dirty;
		};

	public:
		Surface(int width, int height, int depth, Format format, void *pixels, int pitch, int slice);
		Surface(Resource *texture, int width, int height, int depth, Format format, bool lockable, bool renderTarget, int pitchP = 0);

		virtual ~Surface();

		inline void *lock(int x, int y, int z, Lock lock, Accessor client, bool internal = false);
		inline void unlock(bool internal = false);
		inline int getWidth() const;
		inline int getHeight() const;
		inline int getDepth() const;
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

		virtual void *lockInternal(int x, int y, int z, Lock lock, Accessor client);
		virtual void unlockInternal();
		inline Format getInternalFormat() const;
		inline int getInternalPitchB() const;
		inline int getInternalPitchP() const;
		inline int getInternalSliceB() const;
		inline int getInternalSliceP() const;

		void *lockStencil(int front, Accessor client);
		void unlockStencil();
		inline int getStencilPitchB() const;
		inline int getStencilSliceB() const;

		inline int getMultiSampleCount() const;
		inline int getSuperSampleCount() const;

		bool isEntire(const SliceRect& rect) const;
		SliceRect getRect() const;
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

		bool hasStencil() const;
		bool hasDepth() const;
		bool hasPalette() const;
		bool isRenderTarget() const;

		bool hasDirtyMipmaps() const;
		void cleanMipmaps();
		inline bool isExternalDirty() const;
		Resource *getResource();

		static int bytes(Format format);
		static int pitchB(int width, Format format, bool target);
		static int pitchP(int width, Format format, bool target);
		static int sliceB(int width, int height, Format format, bool target);
		static int sliceP(int width, int height, Format format, bool target);
		static unsigned int size(int width, int height, int depth, Format format);   // FIXME: slice * depth

		static bool isStencil(Format format);
		static bool isDepth(Format format);
		static bool isPalette(Format format);

		static bool isFloatFormat(Format format);
		static bool isUnsignedComponent(Format format, int component);
		static bool isSRGBreadable(Format format);
		static bool isSRGBwritable(Format format);
		static bool isCompressed(Format format);
		static bool isNonNormalizedInteger(Format format);
		static int componentCount(Format format);

		static void setTexturePalette(unsigned int *palette);

	protected:
		sw::Resource *resource;

	private:
		typedef unsigned char byte;
		typedef unsigned short word;
		typedef unsigned int dword;
		typedef uint64_t qword;

		#if S3TC_SUPPORT
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
		#endif

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

		static void decodeR8G8B8(Buffer &destination, const Buffer &source);
		static void decodeX1R5G5B5(Buffer &destination, const Buffer &source);
		static void decodeA1R5G5B5(Buffer &destination, const Buffer &source);
		static void decodeX4R4G4B4(Buffer &destination, const Buffer &source);
		static void decodeA4R4G4B4(Buffer &destination, const Buffer &source);
		static void decodeP8(Buffer &destination, const Buffer &source);

		#if S3TC_SUPPORT
		static void decodeDXT1(Buffer &internal, const Buffer &external);
		static void decodeDXT3(Buffer &internal, const Buffer &external);
		static void decodeDXT5(Buffer &internal, const Buffer &external);
		#endif
		static void decodeATI1(Buffer &internal, const Buffer &external);
		static void decodeATI2(Buffer &internal, const Buffer &external);
		static void decodeEAC(Buffer &internal, const Buffer &external, int nbChannels, bool isSigned);
		static void decodeETC2(Buffer &internal, const Buffer &external, int nbAlphaBits, bool isSRGB);
		static void decodeASTC(Buffer &internal, const Buffer &external, int xSize, int ySize, int zSize, bool isSRGB);

		static void update(Buffer &destination, Buffer &source);
		static void genericUpdate(Buffer &destination, Buffer &source);
		static void *allocateBuffer(int width, int height, int depth, Format format);
		static void memfill4(void *buffer, int pattern, int bytes);

		bool identicalFormats() const;
		Format selectInternalFormat(Format format) const;

		void resolve();

		Buffer external;
		Buffer internal;
		Buffer stencil;

		const bool lockable;
		const bool renderTarget;

		bool dirtyMipmaps;
		unsigned int paletteUsed;

		static unsigned int *palette;   // FIXME: Not multi-device safe
		static unsigned int paletteID;

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
		return internal ? getInternalPitchP() : getExternalPitchB();
	}

	int Surface::getSliceB(bool internal) const
	{
		return internal ? getInternalSliceB() : getExternalSliceB();
	}

	int Surface::getSliceP(bool internal) const
	{
		return internal ? getInternalSliceP() : getExternalSliceB();
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

	int Surface::getStencilPitchB() const
	{
		return stencil.pitchB;
	}

	int Surface::getStencilSliceB() const
	{
		return stencil.sliceB;
	}

	int Surface::getMultiSampleCount() const
	{
		return sw::min(internal.depth, 4);
	}

	int Surface::getSuperSampleCount() const
	{
		return internal.depth > 4 ? internal.depth / 4 : 1;
	}

	bool Surface::isExternalDirty() const
	{
		return external.buffer && external.buffer != internal.buffer && external.dirty;
	}
}

#endif   // sw_Surface_hpp
