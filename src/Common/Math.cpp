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

#include "Math.hpp"

#include "CPUID.hpp"

namespace sw
{
	inline uint64_t FNV_1a(uint64_t hash, unsigned char data)
	{
		return (hash ^ data) * 1099511628211;
	}

	uint64_t FNV_1a(const unsigned char *data, int size)
	{
		int64_t hash = 0xCBF29CE484222325;
   
		for(int i = 0; i < size; i++)
		{
			hash = FNV_1a(hash, data[i]);
		}

		return hash;
	}

	unsigned char sRGB8toLinear8(unsigned char value)
	{
		static unsigned char sRGBtoLinearTable[256] = { 255 };
		if(sRGBtoLinearTable[0] == 255)
		{
			for(int i = 0; i < 256; i++)
			{
				sRGBtoLinearTable[i] = static_cast<unsigned char>(sw::sRGBtoLinear(static_cast<float>(i) / 255.0f) * 255.0f + 0.5f);
			}
		}

		return sRGBtoLinearTable[value];
	}
}
