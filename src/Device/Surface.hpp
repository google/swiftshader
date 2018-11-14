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

			VkFormat format;
			AtomicInt lock;

			bool dirty;   // Sibling internal/external buffer doesn't match.
		};

	protected:
		Surface(int width, int height, int depth, VkFormat format, void *pixels, int pitch, int slice);
		Surface(Resource *texture, int width, int height, int depth, int border, int samples, VkFormat format, bool lockable, bool renderTarget, int pitchP = 0);

	public:
		static Surface *create(int width, int height, int depth, VkFormat format, void *pixels, int pitch, int slice);
		static Surface *create(Resource *texture, int width, int height, int depth, int border, int samples, VkFormat format, bool lockable, bool renderTarget, int pitchP = 0);

		virtual ~Surface() = 0;

		inline void *lock(int x, int y, int z, Lock lock, Accessor client, bool internal = false);
		inline void unlock(bool internal = false);
		inline int getWidth() const;
		inline int getHeight() const;
		inline int getDepth() const;
		inline int getBorder() const;
		inline VkFormat getFormat(bool internal = false) const;
		inline int getPitchB(bool internal = false) const;
		inline int getPitchP(bool internal = false) const;
		inline int getSliceB(bool internal = false) const;
		inline int getSliceP(bool internal = false) const;

		void *lockExternal(int x, int y, int z, Lock lock, Accessor client);
		void unlockExternal();
		inline VkFormat getExternalFormat() const;
		inline int getExternalPitchB() const;
		inline int getExternalPitchP() const;
		inline int getExternalSliceB() const;
		inline int getExternalSliceP() const;

		virtual void *lockInternal(int x, int y, int z, Lock lock, Accessor client) = 0;
		virtual void unlockInternal() = 0;
		inline VkFormat getInternalFormat() const;
		inline int getInternalPitchB() const;
		inline int getInternalPitchP() const;
		inline int getInternalSliceB() const;
		inline int getInternalSliceP() const;

		void *lockStencil(int x, int y, int front, Accessor client);
		void unlockStencil();
		inline VkFormat getStencilFormat() const;
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

		static int bytes(VkFormat format);
		static int pitchB(int width, int border, VkFormat format, bool target);
		static int pitchP(int width, int border, VkFormat format, bool target);
		static int sliceB(int width, int height, int border, VkFormat format, bool target);
		static int sliceP(int width, int height, int border, VkFormat format, bool target);
		static size_t size(int width, int height, int depth, int border, int samples, VkFormat format);

		static bool isStencil(VkFormat format);
		static bool isDepth(VkFormat format);
		static bool hasQuadLayout(VkFormat format);

		static bool isFloatFormat(VkFormat format);
		static bool isUnsignedComponent(VkFormat format, int component);
		static bool isSRGBreadable(VkFormat format);
		static bool isSRGBwritable(VkFormat format);
		static bool isSRGBformat(VkFormat format);
		static bool isCompressed(VkFormat format);
		static bool isSignedNonNormalizedInteger(VkFormat format);
		static bool isUnsignedNonNormalizedInteger(VkFormat format);
		static bool isNonNormalizedInteger(VkFormat format);
		static bool isNormalizedInteger(VkFormat format);
		static int componentCount(VkFormat format);

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
		static void *allocateBuffer(int width, int height, int depth, int border, int samples, VkFormat format);
		static void memfill4(void *buffer, int pattern, int bytes);

		bool identicalBuffers() const;
		VkFormat selectInternalFormat(VkFormat format) const;

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

	VkFormat Surface::getFormat(bool internal) const
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

	VkFormat Surface::getExternalFormat() const
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

	VkFormat Surface::getInternalFormat() const
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

	VkFormat Surface::getStencilFormat() const
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
