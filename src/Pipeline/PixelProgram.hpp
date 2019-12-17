// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef sw_PixelProgram_hpp
#define sw_PixelProgram_hpp

#include "PixelRoutine.hpp"

namespace sw {

class PixelProgram : public PixelRoutine
{
public:
	PixelProgram(
	    const PixelProcessor::State &state,
	    vk::PipelineLayout const *pipelineLayout,
	    SpirvShader const *spirvShader,
	    const vk::DescriptorSet::Bindings &descriptorSets)
	    : PixelRoutine(state, pipelineLayout, spirvShader, descriptorSets)
	{
	}

	virtual ~PixelProgram() {}

protected:
	virtual void setBuiltins(Int &x, Int &y, Float4 (&z)[4], Float4 &w, Int cMask[4]);
	virtual void applyShader(Int cMask[4], Int sMask[4], Int zMask[4]);
	virtual Bool alphaTest(Int cMask[4]);
	virtual void rasterOperation(Pointer<Byte> cBuffer[4], Int &x, Int sMask[4], Int zMask[4], Int cMask[4]);

private:
	// Color outputs
	Vector4f c[RENDERTARGETS];

	// Raster operations
	void clampColor(Vector4f oC[RENDERTARGETS]);

	Int4 maskAny(Int cMask[4]) const;
	Int4 maskAny(Int cMask[4], Int sMask[4], Int zMask[4]) const;
	Float4 linearToSRGB(const Float4 &x);
};

}  // namespace sw

#endif
