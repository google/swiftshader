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

#include "Math.hpp"

#include "CPUID.hpp"

namespace sw
{
	const float M_PI     =  3.14159265e+0f;
	const float M_PI_180 =  1.74532925e-2f;
	const float M_180_PI =  5.72957795e+1f;
	const float M_2PI    =  6.28318530e+0f;
	const float M_PI_2   =  1.57079632e+0f;

	int64_t FNV_1(const unsigned char *data, int size)
	{
		int64_t hash = 0xCBF29CE484222325;
   
		for(int i = 0; i < size; i++)
		{
   			hash = hash * 1099511628211;
   			hash = hash ^ data[i];
		}

		return hash;
	}
}
