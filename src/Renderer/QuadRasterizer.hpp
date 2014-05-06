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

#ifndef sw_QuadRasterizer_hpp
#define sw_QuadRasterizer_hpp

#include "Rasterizer.hpp"
#include "PixelRoutine.hpp"

namespace sw
{
	class QuadRasterizer : public PixelRoutine
	{
	public:
		QuadRasterizer(const PixelProcessor::State &state, const PixelShader *pixelShader);

		virtual ~QuadRasterizer();

	private:
		void generate();

		void rasterize(Registers &r, Int &yMin, Int &yMax);
	};
}

#endif   // sw_QuadRasterizer_hpp
