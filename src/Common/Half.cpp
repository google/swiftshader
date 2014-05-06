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

#include "Half.hpp"

namespace sw
{
	half::half(float fp32)
	{
        unsigned int fp32i = *(unsigned int*)&fp32;
        unsigned int sign = (fp32i & 0x80000000) >> 16;
        unsigned int abs = fp32i & 0x7FFFFFFF;

        if(abs > 0x47FFEFFF)   // Infinity
        {
            fp16i = sign | 0x7FFF;
        }
        else if(abs < 0x38800000)   // Denormal
        {
            unsigned int mantissa = (abs & 0x007FFFFF) | 0x00800000;   
            int e = 113 - (abs >> 23);

            if(e < 24)
            {
                abs = mantissa >> e;
            }
            else
            {
                abs = 0;
            }

            fp16i = sign | (abs + 0x00000FFF + ((abs >> 13) & 1)) >> 13;
        }
        else
        {
            fp16i = sign | (abs + 0xC8000000 + 0x00000FFF + ((abs >> 13) & 1)) >> 13;
        }
	}

	half::operator float() const
	{
		unsigned int fp32i;

		int s = (fp16i >> 15) & 0x00000001;
		int e = (fp16i >> 10) & 0x0000001F;
		int m =  fp16i        & 0x000003FF;

		if(e == 0)
		{
			if(m == 0)
			{
				fp32i = s << 31;
			
				return (float&)fp32i;
			}
			else
			{
				while(!(m & 0x00000400))
				{
					m <<= 1;
					e -=  1;
				}

				e += 1;
				m &= ~0x00000400;
			}
		}

		e = e + (127 - 15);
		m = m << 13;

		fp32i = (s << 31) | (e << 23) | m;

		return (float&)fp32i;
	}

	half &half::operator=(half h)
	{
		fp16i = h.fp16i;
		
		return *this;
	}


	half &half::operator=(float f)
	{
		*this = half(f);

		return *this;
	}
}
