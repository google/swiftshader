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

#include "Surface.hpp"
#include "RoutineCache.hpp"
#include "Reactor/Nucleus.hpp"

#include <string.h>

namespace sw
{
	class Blitter
	{
		enum Options : unsigned char
		{
			FILTER_POINT = 0x00,
			WRITE_RED = 0x01,
			WRITE_GREEN = 0x02,
			WRITE_BLUE = 0x04,
			WRITE_ALPHA = 0x08,
			WRITE_RGBA = WRITE_RED | WRITE_GREEN | WRITE_BLUE | WRITE_ALPHA,
			FILTER_LINEAR = 0x10,
			CLEAR_OPERATION = 0x20
		};

		struct BlitState
		{
			bool operator==(const BlitState &state) const
			{
				return memcmp(this, &state, sizeof(BlitState)) == 0;
			}

			Format sourceFormat;
			Format destFormat;
			Blitter::Options options;
		};

		struct BlitData
		{
			void *source;
			void *dest;
			int sPitchB;
			int dPitchB;

			float x0;
			float y0;
			float w;
			float h;

			int y0d;
			int y1d;
			int x0d;
			int x1d;

			int sWidth;
			int sHeight;
		};

	public:
		Blitter();

		virtual ~Blitter();

		void clear(void* pixel, sw::Format format, Surface *dest, const SliceRect &dRect, unsigned int rgbaMask);
		void blit(Surface *source, const SliceRect &sRect, Surface *dest, const SliceRect &dRect, bool filter);
		void blit3D(Surface *source, Surface *dest);

	private:
		bool read(Float4 &color, Pointer<Byte> element, Format format);
		bool write(Float4 &color, Pointer<Byte> element, Format format, const Blitter::Options& options);
		bool read(Int4 &color, Pointer<Byte> element, Format format);
		bool write(Int4 &color, Pointer<Byte> element, Format format, const Blitter::Options& options);
		static bool GetScale(float4& scale, Format format);
		static bool ApplyScaleAndClamp(Float4& value, const BlitState& state);
		void blit(Surface *source, const SliceRect &sRect, Surface *dest, const SliceRect &dRect, const Blitter::Options& options);
		bool blitReactor(Surface *source, const SliceRect &sRect, Surface *dest, const SliceRect &dRect, const Blitter::Options& options);
		Routine *generate(BlitState &state);

		RoutineCache<BlitState> *blitCache;
		BackoffLock criticalSection;
	};

	extern Blitter blitter;
}

#endif   // sw_Blitter_hpp
