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

#ifndef sw_Polygon_hpp
#define sw_Polygon_hpp

#include "Vertex.hpp"

namespace sw
{
	struct Polygon
	{
		Polygon(const float4 *P0, const float4 *P1, const float4 *P2)
		{
			P[0][0] = P0;
			P[0][1] = P1;
			P[0][2] = P2;

			n = 3;
			i = 0;
		}

		Polygon(const float4 *P, int n)
		{
			for(int i = 0; i < n; i++)
			{
				this->P[0][i] = &P[i];
			}

			this->n = n;
			this->i = 0;
		}

		float4 B[16];              // Buffer for clipped vertices
		const float4 *P[16][16];   // Pointers to clipped polygon's vertices

		int i;
		int b;
		int n;
	};
}

#endif   // sw_Polygon_hpp
