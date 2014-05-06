// SwiftShader Software Renderer
//
// Copyright(c) 2005-2012 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef sw_SetupProcessor_hpp
#define sw_SetupProcessor_hpp

#include "Context.hpp"
#include "LRUCache.hpp"
#include "Shader/VertexShader.hpp"
#include "Shader/PixelShader.hpp"
#include "Common/Types.hpp"

namespace sw
{
	struct Primitive;
	struct Triangle;
	struct Polygon;
	struct Vertex;
	class Routine;
	struct DrawCall;
	struct DrawData;

	class SetupProcessor
	{
	public:
		struct States
		{
			unsigned int computeHash();

			unsigned int isDrawPoint         : 1;
			unsigned int isDrawLine          : 1;
			unsigned int isDrawTriangle      : 1;
			unsigned int isDrawSolidTriangle : 1;
			unsigned int interpolateZ        : 1;
			unsigned int interpolateW        : 1;
			unsigned int perspective         : 1;
			unsigned int pointSprite         : 1;
			unsigned int positionRegister    : 4;
			unsigned int pointSizeRegister   : 4;
			unsigned int cullMode            : BITS(Context::CULL_LAST);
			unsigned int twoSidedStencil     : 1;
			unsigned int slopeDepthBias      : 1;
			unsigned int vFace               : 1;
			unsigned int multiSample         : 3;   // 1, 2 or 4

			struct Gradient
			{
				unsigned char attribute : 6;
				unsigned char flat : 1;
				unsigned char wrap : 1;
			};

			union
			{
				struct
				{
					Gradient color[2][4];
					Gradient texture[8][4];
					Gradient fog;
				};

				Gradient gradient[10][4];
			};
		};

		struct State : States
		{
			State(int i = 0);

			bool operator==(const State &states) const;

			unsigned int hash;
		};

		typedef bool (__cdecl *RoutinePointer)(Primitive *primitive, const Triangle *triangle, const Polygon *polygon, const DrawData *draw);

		SetupProcessor(Context *context);

		~SetupProcessor();

	protected:
		State update() const;
		Routine *routine(const State &state);

		void setRoutineCacheSize(int cacheSize);

		float depthBias;
		float slopeDepthBias;

	private:
		Context *const context;

		LRUCache<State, Routine> *routineCache;
		HMODULE precacheDLL;
	};
}

#endif   // sw_SetupProcessor_hpp
