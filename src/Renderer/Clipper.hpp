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

#ifndef sw_Clipper_hpp
#define sw_Clipper_hpp

#include "Plane.hpp"
#include "Common/Types.hpp"

namespace sw
{
	struct Polygon;
	struct DrawCall;
	struct DrawData;

	class Clipper
	{
	public:
		enum ClipFlags
		{
			CLIP_RIGHT  = 1 << 0,
			CLIP_TOP    = 1 << 1,
			CLIP_FAR    = 1 << 2,
			CLIP_LEFT   = 1 << 3,
			CLIP_BOTTOM = 1 << 4,
			CLIP_NEAR   = 1 << 5,

			CLIP_FINITE = 1 << 7,

			// User-defined clipping planes
			CLIP_PLANE0	= 1 << 8,
			CLIP_PLANE1	= 1 << 9,
			CLIP_PLANE2	= 1 << 10,
			CLIP_PLANE3	= 1 << 11,
			CLIP_PLANE4	= 1 << 12,
			CLIP_PLANE5	= 1 << 13
		};

		Clipper();

		~Clipper();

		bool clip(Polygon &polygon, int clipFlagsOr, const DrawCall &draw);

	private:
		void clipNear(Polygon &polygon);
		void clipFar(Polygon &polygon);
		void clipLeft(Polygon &polygon, const DrawData &data);
		void clipRight(Polygon &polygon, const DrawData &data);
		void clipTop(Polygon &polygon, const DrawData &data);
		void clipBottom(Polygon &polygon, const DrawData &data);
		void clipPlane(Polygon &polygon, const Plane &plane);

		void clipEdge(float4 &Vo, const float4 &Vi, const float4 &Vj, float di, float dj) const;
	};
}

#endif   // sw_Clipper_hpp
