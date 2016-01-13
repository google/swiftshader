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
		Rasterizer(const PixelProcessor::State &state);

		virtual ~Rasterizer();

		virtual void generate() = 0;

	protected:
		const PixelProcessor::State &state;
	};
}

#endif   // sw_Rasterizer_hpp
