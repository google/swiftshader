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

#include "Plane.hpp"

#include "Matrix.hpp"

namespace sw
{
	Plane::Plane()
	{
	}

	Plane::Plane(float p_A, float p_B, float p_C, float p_D)
	{
		A = p_A;
		B = p_B;
		C = p_C;
		D = p_D;
	}

	Plane::Plane(const float ABCD[4])
	{
		A = ABCD[0];
		B = ABCD[1];
		C = ABCD[2];
		D = ABCD[3];
	}

	Plane operator*(const Plane &p, const Matrix &T)
	{
		Matrix M = !T;

		return Plane(p.A * M(1, 1) + p.B * M(1, 2) + p.C * M(1, 3) + p.D * M(1, 4),
		             p.A * M(2, 1) + p.B * M(2, 2) + p.C * M(2, 3) + p.D * M(2, 4),
		             p.A * M(3, 1) + p.B * M(3, 2) + p.C * M(3, 3) + p.D * M(3, 4),
		             p.A * M(4, 1) + p.B * M(4, 2) + p.C * M(4, 3) + p.D * M(4, 4));
	}

	Plane operator*(const Matrix &T, const Plane &p)
	{
		Matrix M = !T;

		return Plane(M(1, 1) * p.A + M(2, 1) * p.B + M(3, 1) * p.C + M(4, 1) * p.D,
		             M(1, 2) * p.A + M(2, 2) * p.B + M(3, 2) * p.C + M(4, 2) * p.D,
		             M(1, 3) * p.A + M(2, 3) * p.B + M(3, 3) * p.C + M(4, 3) * p.D,
		             M(1, 4) * p.A + M(2, 4) * p.B + M(3, 4) * p.C + M(4, 4) * p.D);
	}
}
