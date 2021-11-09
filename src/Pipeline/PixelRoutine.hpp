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

#include <vector>

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
	using SampleSet = std::vector<int>;

	Float4 z[4];  // Multisampled z
	Float4 w;     // Used as is
	Float4 rhw;   // Reciprocal w

	SpirvRoutine routine;
	const vk::DescriptorSet::Bindings &descriptorSets;

	virtual void setBuiltins(Int &x, Int &y, Float4 (&z)[4], Float4 &w, Int cMask[4], const SampleSet &samples) = 0;
	virtual void executeShader(Int cMask[4], Int sMask[4], Int zMask[4], const SampleSet &samples) = 0;
	virtual Bool alphaTest(Int cMask[4], const SampleSet &samples) = 0;
	virtual void blendColor(Pointer<Byte> cBuffer[4], Int &x, Int sMask[4], Int zMask[4], Int cMask[4], const SampleSet &samples) = 0;

	void quad(Pointer<Byte> cBuffer[4], Pointer<Byte> &zBuffer, Pointer<Byte> &sBuffer, Int cMask[4], Int &x, Int &y) override;

	void alphaTest(Int &aMask, const Short4 &alpha);
	void alphaToCoverage(Int cMask[4], const Float4 &alpha, const SampleSet &samples);

	void writeColor(int index, const Pointer<Byte> &cBuffer, const Int &x, Vector4f &color, const Int &sMask, const Int &zMask, const Int &cMask);
	Vector4f alphaBlend(int index, const Pointer<Byte> &cBuffer, const Vector4f &sourceColor, const Int &x);
	void writeColor(int index, const Pointer<Byte> &cBuffer, const Int &x, Vector4s &current, const Int &sMask, const Int &zMask, const Int &cMask);

	bool isSRGB(int index) const;
	UShort4 convertFixed16(const Float4 &cf, bool saturate = true);
	Float4 convertFloat32(const UShort4 &cf);
	void linearToSRGB12_16(Vector4s &c);

private:
	Byte8 stencilReplaceRef(bool isBack);
	void stencilTest(const Pointer<Byte> &sBuffer, const Int &x, Int sMask[4], const SampleSet &samples);
	void stencilTest(Byte8 &value, VkCompareOp stencilCompareMode, bool isBack);
	void stencilOperation(Byte8 &newValue, const Byte8 &bufferValue, const PixelProcessor::States::StencilOpState &ops, bool isBack, const Int &zMask, const Int &sMask);
	void stencilOperation(Byte8 &output, const Byte8 &bufferValue, VkStencilOp operation, bool isBack);
	Float4 clampDepth(const Float4 &z);
	Bool depthTest(const Pointer<Byte> &zBuffer, int q, const Int &x, const Float4 &z, const Int &sMask, Int &zMask, const Int &cMask);
	void depthBoundsTest(const Pointer<Byte> &zBuffer, int q, const Int &x, Int &zMask, Int &cMask);

	void readPixel(int index, const Pointer<Byte> &cBuffer, const Int &x, Vector4s &pixel);
	enum BlendFactorModifier { None, OneMinus };
	Float blendConstant(vk::Format format, int component, BlendFactorModifier modifier = None);
	void blendFactorRGB(Vector4f &blendFactorRGB, const Vector4f &sourceColor, const Vector4f &destColor, VkBlendFactor colorBlendFactor, vk::Format format);
	void blendFactorAlpha(Float4 &blendFactorAlpha, const Float4 &sourceAlpha, const Float4 &destAlpha, VkBlendFactor alphaBlendFactor, vk::Format format);
	bool blendFactorCanExceedFormatRange(VkBlendFactor blendFactor, vk::Format format);
	Vector4f computeAdvancedBlendMode(int index, const Vector4f &src, const Vector4f &dst, const Vector4f &srcFactor, const Vector4f &dstFactor);
	Float4 blendOpOverlay(Float4 &src, Float4 &dst);
	Float4 blendOpColorDodge(Float4 &src, Float4 &dst);
	Float4 blendOpColorBurn(Float4 &src, Float4 &dst);
	Float4 blendOpHardlight(Float4 &src, Float4 &dst);
	Float4 blendOpSoftlight(Float4 &src, Float4 &dst);
	void setLumSat(Vector4f &cbase, Vector4f &csat, Vector4f &clum, Float4 &x, Float4 &y, Float4 &z);
	void setLum(Vector4f &cbase, Vector4f &clum, Float4 &x, Float4 &y, Float4 &z);
	Float4 computeLum(Float4 &color, Float4 &lum, Float4 &mincol, Float4 &maxcol, Int4 &negative, Int4 &aboveOne);
	Float4 maxRGB(Vector4f &c);
	Float4 minRGB(Vector4f &c);
	Float4 lumRGB(Vector4f &c);
	void premultiply(Vector4f &c);
	void writeStencil(Pointer<Byte> &sBuffer, const Int &x, const Int sMask[4], const Int zMask[4], const Int cMask[4], const SampleSet &samples);
	void writeDepth(Pointer<Byte> &zBuffer, const Int &x, const Int zMask[4], const SampleSet &samples);
	void occlusionSampleCount(const Int zMask[4], const Int sMask[4], const SampleSet &samples);

	void sRGBtoLinear16_12_16(Vector4s &c);
	void linearToSRGB16_12_16(Vector4s &c);
	Float4 sRGBtoLinear(const Float4 &x);

	Bool depthTest32F(const Pointer<Byte> &zBuffer, int q, const Int &x, const Float4 &z, const Int &sMask, Int &zMask, const Int &cMask);
	Bool depthTest16(const Pointer<Byte> &zBuffer, int q, const Int &x, const Float4 &z, const Int &sMask, Int &zMask, const Int &cMask);

	void writeDepth32F(Pointer<Byte> &zBuffer, int q, const Int &x, const Float4 &z, const Int &zMask);
	void writeDepth16(Pointer<Byte> &zBuffer, int q, const Int &x, const Float4 &z, const Int &zMask);

	Int4 depthBoundsTest32F(const Pointer<Byte> &zBuffer, int q, const Int &x);
	Int4 depthBoundsTest16(const Pointer<Byte> &zBuffer, int q, const Int &x);

	// Derived state parameters
	const bool shaderContainsInterpolation;  // TODO(b/194714095)
	const bool shaderContainsSampleQualifier;
	const bool perSampleShading;
	const int invocationCount;

	SampleSet getSampleSet(int invocation) const;
};

}  // namespace sw

#endif  // sw_PixelRoutine_hpp
