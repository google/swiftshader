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

#ifndef sw_Blitter_hpp
#define sw_Blitter_hpp

#include "Memset.hpp"
#include "RoutineCache.hpp"
#include "Reactor/Reactor.hpp"
#include "Vulkan/VkFormat.h"

#include <cstring>
#include <mutex>

namespace vk {

class Image;
class Buffer;

}  // namespace vk

namespace sw {

class Blitter
{
	struct Options
	{
		explicit Options() = default;
		explicit Options(bool filter, bool allowSRGBConversion)
		    : writeMask(0xF)
		    , clearOperation(false)
		    , filter(filter)
		    , allowSRGBConversion(allowSRGBConversion)
		    , clampToEdge(false)
		{}
		explicit Options(unsigned int writeMask)
		    : writeMask(writeMask)
		    , clearOperation(true)
		    , filter(false)
		    , allowSRGBConversion(true)
		    , clampToEdge(false)
		{}

		union
		{
			struct
			{
				bool writeRed : 1;
				bool writeGreen : 1;
				bool writeBlue : 1;
				bool writeAlpha : 1;
			};

			unsigned char writeMask;
		};

		bool clearOperation : 1;
		bool filter : 1;
		bool allowSRGBConversion : 1;
		bool clampToEdge : 1;
	};

	struct State : Memset<State>, Options
	{
		State()
		    : Memset(this, 0)
		{}
		State(const Options &options)
		    : Memset(this, 0)
		    , Options(options)
		{}
		State(vk::Format sourceFormat, vk::Format destFormat, int srcSamples, int destSamples, const Options &options)
		    : Memset(this, 0)
		    , Options(options)
		    , sourceFormat(sourceFormat)
		    , destFormat(destFormat)
		    , srcSamples(srcSamples)
		    , destSamples(destSamples)
		{}

		vk::Format sourceFormat;
		vk::Format destFormat;
		int srcSamples = 0;
		int destSamples = 0;
		bool filter3D = false;
	};

	struct BlitData
	{
		void *source;
		void *dest;
		int sPitchB;
		int dPitchB;
		int sSliceB;
		int dSliceB;

		float x0;
		float y0;
		float z0;
		float w;
		float h;
		float d;

		int x0d;
		int x1d;
		int y0d;
		int y1d;
		int z0d;
		int z1d;

		int sWidth;
		int sHeight;
		int sDepth;

		bool filter3D;
	};

	struct CubeBorderData
	{
		void *layers;
		int pitchB;
		uint32_t layerSize;
		uint32_t dim;
	};

public:
	Blitter();
	virtual ~Blitter();

	void clear(void *pixel, vk::Format format, vk::Image *dest, const vk::Format &viewFormat, const VkImageSubresourceRange &subresourceRange, const VkRect2D *renderArea = nullptr);

	void blit(const vk::Image *src, vk::Image *dst, VkImageBlit region, VkFilter filter);
	void blitToBuffer(const vk::Image *src, VkImageSubresourceLayers subresource, VkOffset3D offset, VkExtent3D extent, uint8_t *dst, int bufferRowPitch, int bufferSlicePitch);
	void blitFromBuffer(const vk::Image *dst, VkImageSubresourceLayers subresource, VkOffset3D offset, VkExtent3D extent, uint8_t *src, int bufferRowPitch, int bufferSlicePitch);

	void updateBorders(vk::Image *image, const VkImageSubresourceLayers &subresourceLayers);

private:
	enum Edge
	{
		TOP,
		BOTTOM,
		RIGHT,
		LEFT
	};

	bool fastClear(void *pixel, vk::Format format, vk::Image *dest, const vk::Format &viewFormat, const VkImageSubresourceRange &subresourceRange, const VkRect2D *renderArea);

	Float4 readFloat4(Pointer<Byte> element, const State &state);
	void write(Float4 &color, Pointer<Byte> element, const State &state);
	Int4 readInt4(Pointer<Byte> element, const State &state);
	void write(Int4 &color, Pointer<Byte> element, const State &state);
	static void ApplyScaleAndClamp(Float4 &value, const State &state, bool preScaled = false);
	static Int ComputeOffset(Int &x, Int &y, Int &pitchB, int bytes);
	static Int ComputeOffset(Int &x, Int &y, Int &z, Int &sliceB, Int &pitchB, int bytes);
	static Float4 LinearToSRGB(const Float4 &color);
	static Float4 sRGBtoLinear(const Float4 &color);

	using BlitFunction = FunctionT<void(const BlitData *)>;
	using BlitRoutineType = BlitFunction::RoutineType;
	BlitRoutineType getBlitRoutine(const State &state);
	BlitRoutineType generate(const State &state);
	Float4 sample(Pointer<Byte> &source, Float &x, Float &y, Float &z,
	              Int &sWidth, Int &sHeight, Int &sDepth,
	              Int &sSliceB, Int &sPitchB, const State &state);

	using CornerUpdateFunction = FunctionT<void(const CubeBorderData *)>;
	using CornerUpdateRoutineType = CornerUpdateFunction::RoutineType;
	CornerUpdateRoutineType getCornerUpdateRoutine(const State &state);
	CornerUpdateRoutineType generateCornerUpdate(const State &state);
	void computeCubeCorner(Pointer<Byte> &layer, Int &x0, Int &x1, Int &y0, Int &y1, Int &pitchB, const State &state);

	void copyCubeEdge(vk::Image *image,
	                  const VkImageSubresourceLayers &dstSubresourceLayers, Edge dstEdge,
	                  const VkImageSubresourceLayers &srcSubresourceLayers, Edge srcEdge);

	std::mutex blitMutex;
	RoutineCacheT<State, BlitFunction::CFunctionType> blitCache;  // guarded by blitMutex
	std::mutex cornerUpdateMutex;
	RoutineCacheT<State, CornerUpdateFunction::CFunctionType> cornerUpdateCache;  // guarded by cornerUpdateMutex
};

}  // namespace sw

#endif  // sw_Blitter_hpp
