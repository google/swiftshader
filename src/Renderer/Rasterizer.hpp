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

#ifndef sw_Rasterizer_hpp
#define sw_Rasterizer_hpp

#include "Context.hpp"

#include "PixelProcessor.hpp"
#include "Config.hpp"

namespace sw
{
	class Rasterizer : public Function<Void(Pointer<Byte>, Int, Int, Pointer<Byte>)>
	{
	public:
		Rasterizer() : primitive(Arg<0>()), count(Arg<1>()), cluster(Arg<2>()), data(Arg<3>()) {}
		virtual ~Rasterizer() {};

	protected:
		const Pointer<Byte> primitive;
		const Int count;
		const Int cluster;
		const Pointer<Byte> data;
	};
}

#endif   // sw_Rasterizer_hpp
