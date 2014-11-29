// SwiftShader Software Renderer
//
// Copyright(c) 2005-2013 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

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
		void clip(int minX, int minY, int maxX, int maxY);

		int x0;   // Inclusive
		int y0;   // Inclusive
		int x1;   // Exclusive
		int y1;   // Exclusive
	};

	enum Format : unsigned char
	{
		FORMAT_NULL,

		FORMAT_A8,
		FORMAT_R8,
		FORMAT_R3G3B2,
		FORMAT_A8R3G3B2,
		FORMAT_X4R4G4B4,
		FORMAT_A4R4G4B4,
		FORMAT_R5G6B5,
		FORMAT_R8G8B8,
		FORMAT_X8R8G8B8,
		FORMAT_A8R8G8B8,
		FORMAT_X8B8G8R8,
		FORMAT_A8B8G8R8,
		FORMAT_X1R5G5B5,
		FORMAT_A1R5G5B5,
		FORMAT_G8R8,
		FORMAT_G16R16,
		FORMAT_A2R10G10B10,
		FORMAT_A2B10G10R10,
		FORMAT_A16B16G16R16,
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
		// Floating-point formats
		FORMAT_R16F,
		FORMAT_G16R16F,
		FORMAT_A16B16G16R16F,
		FORMAT_R32F,
		FORMAT_G32R32F,
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

		FORMAT_LAST = FORMAT_A8G8R8B8Q
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
		Surface(Resource *texture, int width, int height, int depth, Format format, bool lockable, bool renderTarget);
		
		virtual ~Surface();

		void *lockExternal(int x, int y, int z, Lock lock, Accessor client);
		void unlockExternal();
		inline int getExternalWidth() const;
		inline int getExternalHeight() const;
		inline int getExternalDepth() const;
		inline Format getExternalFormat() const;
		inline int getExternalPitchB() const;
		inline int getExternalPitchP() const;
		inline int getExternalSliceB() const;
		inline int getExternalSliceP() const;

		virtual void *lockInternal(int x, int y, int z, Lock lock, Accessor client);
		virtual void unlockInternal();
		inline int getInternalWidth() const;
		inline int getInternalHeight() const;
		inline int getInternalDepth() const;
		inline Format getInternalFormat() const;
		inline int getInternalPitchB() const;
		inline int getInternalPitchP() const;
		inline int getInternalSliceB() const;
		inline int getInternalSliceP() const;

		void *lockStencil(int front, Accessor client);
		void unlockStencil();
		inline int getStencilPitchB() const;
		inline int getStencilPitchP() const;
		inline int getStencilSliceB() const;

		inline int getMultiSampleCount() const;
		inline int getSuperSampleCount() const;

		void clearColorBuffer(unsigned int color, unsigned int rgbaMask, int x0, int y0, int width, int height);
		void clearDepthBuffer(float depth, int x0, int y0, int width, int height);
		void clearStencilBuffer(unsigned char stencil, unsigned char mask, int x0, int y0, int width, int height);
		void fill(const Color<float> &color, int x0, int y0, int width, int height);

		Color<float> readExternal(int x, int y, int z) const;
		Color<float> readExternal(int x, int y) const;
		Color<float> sampleExternal(float x, float y, float z) const;
		Color<float> sampleExternal(float x, float y) const;
		void writeExternal(int x, int y, int z, const Color<float> &color);
		void writeExternal(int x, int y, const Color<float> &color);
		
		Color<float> readInternal(int x, int y, int z) const;
		Color<float> readInternal(int x, int y) const;
		Color<float> sampleInternal(float x, float y, float z) const;
		Color<float> sampleInternal(float x, float y) const;
		void writeInternal(int x, int y, int z, const Color<float> &color);
		void writeInternal(int x, int y, const Color<float> &color);

		bool hasStencil() const;
		bool hasDepth() const;
		bool hasPalette() const;
		bool isRenderTarget() const;

		bool hasDirtyMipmaps() const;
		void cleanMipmaps();
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
		static int componentCount(Format format);

		static void setTexturePalette(unsigned int *palette);

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
		static void decodeX8B8G8R8(Buffer &destination, const Buffer &source);
		static void decodeA8B8G8R8(Buffer &destination, const Buffer &source);
		static void decodeR5G6B5(Buffer &destination, const Buffer &source);
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
		static void decodeETC1(Buffer &internal, const Buffer &external);

		static void update(Buffer &destination, Buffer &source);
		static void genericUpdate(Buffer &destination, Buffer &source);
		static void *allocateBuffer(int width, int height, int depth, Format format);
		static void memfill(void *buffer, int pattern, int bytes);

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

		sw::Resource *resource;
		bool hasParent;
	};
}

#undef min
#undef max

namespace sw
{
	int Surface::getExternalWidth() const
	{
		return external.width;
	}

	int Surface::getExternalHeight() const
	{
		return external.height;
	}

	int Surface::getExternalDepth() const
	{
		return external.depth;
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

	int Surface::getInternalWidth() const
	{
		return internal.width;
	}

	int Surface::getInternalHeight() const
	{
		return internal.height;
	}

	int Surface::getInternalDepth() const
	{
		return internal.depth;
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

	int Surface::getStencilPitchP() const
	{
		return stencil.pitchP;
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
}

#endif   // sw_Surface_hpp
