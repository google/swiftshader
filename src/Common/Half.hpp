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

#ifndef sw_Half_hpp
#define sw_Half_hpp

namespace sw
{
	class half
	{
	public:
		explicit half(float f);

		operator float() const;

		half &operator=(half h);
		half &operator=(float f);

	private:
		unsigned short fp16i;
	};
}

#endif   // sw_Half_hpp
