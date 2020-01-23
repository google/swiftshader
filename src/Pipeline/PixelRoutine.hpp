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

#ifndef sw_PixelRoutine_hpp
#define sw_PixelRoutine_hpp

#include "Device/QuadRasterizer.hpp"

namespace sw {

class PixelShader;
class SamplerCore;

class PixelRoutine : public sw::QuadRasterizer
{
public:
	PixelRoutine(const PixelProcessor::State &state,
	             vk::PipelineLayout const *pipelineLayout,
	             SpirvShader const *spirvShader,
	             const vk::DescriptorSet::Bindings &descriptorSets);

	virtual ~PixelRoutine();

protected:
	Float4 z[4];  // Multisampled z
	Float4 w;     // Used as is
	Float4 rhw;   // Reciprocal w

	SpirvRoutine routine;
	const vk::DescriptorSet::Bindings &descriptorSets;

	// Depth output
	Float4 oDepth;

	virtual void setBuiltins(Int &x, Int &y, Float4 (&z)[4], Float4 &w, Int cMask[4]) = 0;
	virtual void applyShader(Int cMask[4], Int sMask[4], Int zMask[4]) = 0;
	virtual Bool alphaTest(Int cMask[4]) = 0;
	virtual void rasterOperation(Pointer<Byte> cBuffer[4], Int &x, Int sMask[4], Int zMask[4], Int cMask[4]) = 0;

	void quad(Pointer<Byte> cBuffer[4], Pointer<Byte> &zBuffer, Pointer<Byte> &sBuffer, Int cMask[4], Int &x, Int &y) override;

	void alphaTest(Int &aMask, const Short4 &alpha);
	void alphaToCoverage(Int cMask[4], const Float4 &alpha);

	// Raster operations
	void alphaBlend(int index, const Pointer<Byte> &cBuffer, Vector4s &current, const Int &x);
	void writeColor(int index, const Pointer<Byte> &cBuffer, const Int &x, Vector4f &oC, const Int &sMask, const Int &zMask, const Int &cMask);
	void alphaBlend(int index, const Pointer<Byte> &cBuffer, Vector4f &oC, const Int &x);
	void writeColor(int index, const Pointer<Byte> &cBuffer, const Int &x, Vector4s &current, const Int &sMask, const Int &zMask, const Int &cMask);

	bool isSRGB(int index) const;
	UShort4 convertFixed16(const Float4 &cf, bool saturate = true);
	void linearToSRGB12_16(Vector4s &c);

private:
	Float4 interpolateCentroid(const Float4 &x, const Float4 &y, const Float4 &rhw, Pointer<Byte> planeEquation, bool flat, bool perspective);
	Byte8 stencilReplaceRef(bool isBack);
	void stencilTest(const Pointer<Byte> &sBuffer, int q, const Int &x, Int &sMask, const Int &cMask);
	void stencilTest(Byte8 &value, VkCompareOp stencilCompareMode, bool isBack);
	void stencilOperation(Byte8 &newValue, const Byte8 &bufferValue, const PixelProcessor::States::StencilOpState &ops, bool isBack, const Int &zMask, const Int &sMask);
	void stencilOperation(Byte8 &output, const Byte8 &bufferValue, VkStencilOp operation, bool isBack);
	Bool depthTest(const Pointer<Byte> &zBuffer, int q, const Int &x, const Float4 &z, const Int &sMask, Int &zMask, const Int &cMask);

	// Raster operations
	void blendFactor(Vector4s &blendFactor, const Vector4s &current, const Vector4s &pixel, VkBlendFactor blendFactorActive);
	void blendFactorAlpha(Vector4s &blendFactor, const Vector4s &current, const Vector4s &pixel, VkBlendFactor blendFactorAlphaActive);
	void readPixel(int index, const Pointer<Byte> &cBuffer, const Int &x, Vector4s &pixel);
	void blendFactor(Vector4f &blendFactor, const Vector4f &oC, const Vector4f &pixel, VkBlendFactor blendFactorActive);
	void blendFactorAlpha(Vector4f &blendFactor, const Vector4f &oC, const Vector4f &pixel, VkBlendFactor blendFactorAlphaActive);
	void writeStencil(Pointer<Byte> &sBuffer, int q, const Int &x, const Int &sMask, const Int &zMask, const Int &cMask);
	void writeDepth(Pointer<Byte> &zBuffer, int q, const Int &x, const Float4 &z, const Int &zMask);

	void sRGBtoLinear16_12_16(Vector4s &c);
	void linearToSRGB16_12_16(Vector4s &c);
	Float4 sRGBtoLinear(const Float4 &x);

	Bool depthTest32F(const Pointer<Byte> &zBuffer, int q, const Int &x, const Float4 &z, const Int &sMask, Int &zMask, const Int &cMask);
	Bool depthTest16(const Pointer<Byte> &zBuffer, int q, const Int &x, const Float4 &z, const Int &sMask, Int &zMask, const Int &cMask);

	void writeDepth32F(Pointer<Byte> &zBuffer, int q, const Int &x, const Float4 &z, const Int &zMask);
	void writeDepth16(Pointer<Byte> &zBuffer, int q, const Int &x, const Float4 &z, const Int &zMask);
};

}  // namespace sw

#endif  // sw_PixelRoutine_hpp
