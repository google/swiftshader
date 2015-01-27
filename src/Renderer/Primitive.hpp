// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef sw_Primitive_hpp
#define sw_Primitive_hpp

#include "Vertex.hpp"
#include "Config.hpp"

namespace sw
{
	struct Triangle
	{
		Vertex v0;
		Vertex v1;
		Vertex v2;
	};

	struct PlaneEquation   // z = A * x + B * y + C
	{
		float4 A;
		float4 B;
		float4 C;
	};

	struct Primitive
	{
		int yMin;
		int yMax;

		float4 xQuad;
		float4 yQuad;

		PlaneEquation z;
		PlaneEquation w;

		union
		{
			struct
			{
				PlaneEquation C[2][4];
				PlaneEquation T[8][4];
			};

			PlaneEquation V[10][4];
		};

		PlaneEquation f;

		float area;

		// Masks for two-sided stencil
		int64_t clockwiseMask;
		int64_t invClockwiseMask;

		struct Span
		{
			unsigned short left;
			unsigned short right;
		};

		Span outlineUnderflow;
		Span outline[OUTLINE_RESOLUTION];
		Span outlineOverflow;
	};
}

#endif   // sw_Primitive_hpp
