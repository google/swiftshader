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
#include "RoutineCache.hpp"
#include "Shader/VertexShader.hpp"
#include "Shader/PixelShader.hpp"
#include "Common/Types.hpp"

namespace sw
{
	struct Primitive;
	struct Triangle;
	struct Polygon;
	struct Vertex;
	struct DrawCall;
	struct DrawData;

	class SetupProcessor
	{
	public:
		struct States
		{
			unsigned int computeHash();

			bool isDrawPoint               : 1;
			bool isDrawLine                : 1;
			bool isDrawTriangle            : 1;
			bool isDrawSolidTriangle       : 1;
			bool interpolateZ              : 1;
			bool interpolateW              : 1;
			bool perspective               : 1;
			bool pointSprite               : 1;
			unsigned int positionRegister  : 4;
			unsigned int pointSizeRegister : 4;
			CullMode cullMode              : BITS(CULL_LAST);
			bool twoSidedStencil           : 1;
			bool slopeDepthBias            : 1;
			bool vFace                     : 1;
			unsigned int multiSample       : 3;   // 1, 2 or 4
			bool rasterizerDiscard         : 1;

			struct Gradient
			{
				unsigned char attribute : BITS(Unused);
				bool flat               : 1;
				bool wrap               : 1;
			};

			union
			{
				struct
				{
					Gradient color[2][4];
					Gradient texture[8][4];
				};

				Gradient gradient[10][4];
			};

			Gradient fog;
		};

		struct State : States
		{
			State(int i = 0);

			bool operator==(const State &states) const;

			unsigned int hash;
		};

		typedef bool (*RoutinePointer)(Primitive *primitive, const Triangle *triangle, const Polygon *polygon, const DrawData *draw);

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

		RoutineCache<State> *routineCache;
	};
}

#endif   // sw_SetupProcessor_hpp
