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

#ifndef sw_Triangle_hpp
#define sw_Triangle_hpp

#include "Vertex.hpp"

namespace sw
{
	struct Triangle
	{
		Vertex V0;
		Vertex V1;
		Vertex V2;
	};
}

#endif   // sw_Triangle_hpp
