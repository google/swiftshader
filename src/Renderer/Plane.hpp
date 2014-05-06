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

	struct Plane
	{
		float A;
		float B;
		float C;
		float D;

		Plane();
		Plane(float A, float B, float C, float D);   // Plane equation 
		Plane(const float ABCD[4]);

		friend Plane operator*(const Plane &p, const Matrix &A);   // Transform plane by matrix (post-multiply)
		friend Plane operator*(const Matrix &A, const Plane &p);   // Transform plane by matrix (pre-multiply)
	};
}

#endif   // Plane_hpp
