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

#ifndef Plane_hpp
#define Plane_hpp

#include "Vector.hpp"

namespace sw
{
	struct Matrix;
	struct Point;

	struct Plane
	{
		union
		{
			struct
			{
				float A;
				float B;
				float C;
			};
			struct
			{
				Vector n;
			};
		};

		float D;   // Distance to origin along normal

		Plane();
		Plane(const Plane &p);
		Plane(const Vector &n, float D);   // Normal and distance to origin
		Plane(const Vector &n, const Point &P);   // Normal and point on plane
		Plane(const Point &P0, const Point &P1, const Point &P2);   // Through three points
		Plane(float A, float B, float C, float D);   // Plane equation 
		Plane(const float ABCD[4]);

		Plane &operator=(const Plane &p);

		Plane operator+() const;
		Plane operator-() const;   // Flip normal

		Plane &operator*=(const Matrix &A);   // Transform plane by matrix (post-multiply)

		friend Plane operator*(const Plane &p, const Matrix &A);   // Transform plane by matrix (post-multiply)
		friend Plane operator*(const Matrix &A, const Plane &p);   // Transform plane by matrix (pre-multiply)

		friend float operator^(const Plane &p1, const Plane &p2);   // Angle between planes

		float d(const Point &P) const;   // Oriented distance between point and plane

		static float d(const Point &P, const Plane &p);   // Oriented distance between point and plane
		static float d(const Plane &p, const Point &P);   // Oriented distance between plane and point

		Plane &normalise();   // Normalise the Plane equation
	};
}

#endif   // Plane_hpp
