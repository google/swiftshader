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

#ifndef sw_SamplerCore_hpp
#define sw_SamplerCore_hpp

#include "ShaderCore.hpp"
#include "Device/Sampler.hpp"
#include "Reactor/Print.hpp"
#include "Reactor/Reactor.hpp"

#ifdef None
#	undef None  // b/127920555
#endif

namespace sw {

using namespace rr;

enum SamplerMethod : uint32_t
{
	Implicit,  // Compute gradients (pixel shader only).
	Bias,      // Compute gradients and add provided bias.
	Lod,       // Use provided LOD.
	Grad,      // Use provided gradients.
	Fetch,     // Use provided integer coordinates.
	Base,      // Sample base level.
	Query,     // Return implicit LOD.
	Gather,    // Return one channel of each texel in footprint.
	SAMPLER_METHOD_LAST = Gather,
};

// TODO(b/129523279): Eliminate and use SpirvShader::ImageInstruction instead.
struct SamplerFunction
{
	SamplerFunction(SamplerMethod method, bool offset = false, bool sample = false)
	    : method(method)
	    , offset(offset)
	    , sample(sample)
	{}

	operator SamplerMethod() { return method; }

	const SamplerMethod method;
	const bool offset;
	const bool sample;
};

class SamplerCore
{
public:
	SamplerCore(Pointer<Byte> &constants, const Sampler &state);

	Vector4f sampleTexture(Pointer<Byte> &texture, Float4 uvw[4], Float4 &q, Float &&lodOrBias, Float4 &dsx, Float4 &dsy, Vector4f &offset, Int4 &sampleId, SamplerFunction function);

private:
	Short4 offsetSample(Short4 &uvw, Pointer<Byte> &mipmap, int halfOffset, bool wrap, int count, Float &lod);
	Vector4s sampleFilter(Pointer<Byte> &texture, Float4 &u, Float4 &v, Float4 &w, Vector4f &offset, const Float4 &cubeArrayCoord, const Int4 &sampleId, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, SamplerFunction function);
	Vector4s sampleAniso(Pointer<Byte> &texture, Float4 &u, Float4 &v, Float4 &w, Vector4f &offset, const Float4 &cubeArrayCoord, const Int4 &sampleId, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, bool secondLOD, SamplerFunction function);
	Vector4s sampleQuad(Pointer<Byte> &texture, Float4 &u, Float4 &v, Float4 &w, Vector4f &offset, const Float4 &cubeArrayCoord, const Int4 &sampleId, Float &lod, bool secondLOD, SamplerFunction function);
	Vector4s sampleQuad2D(Pointer<Byte> &texture, Float4 &u, Float4 &v, Float4 &w, Vector4f &offset, const Float4 &cubeArrayCoord, const Int4 &sampleId, Float &lod, bool secondLOD, SamplerFunction function);
	Vector4s sample3D(Pointer<Byte> &texture, Float4 &u, Float4 &v, Float4 &w, Vector4f &offset, const Float4 &cubeArrayCoord, const Int4 &sampleId, Float &lod, bool secondLOD, SamplerFunction function);
	Vector4f sampleFloatFilter(Pointer<Byte> &texture, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &offset, const Float4 &cubeArrayCoord, const Int4 &sampleId, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, SamplerFunction function);
	Vector4f sampleFloatAniso(Pointer<Byte> &texture, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &offset, const Float4 &cubeArrayCoord, const Int4 &sampleId, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, bool secondLOD, SamplerFunction function);
	Vector4f sampleFloat(Pointer<Byte> &texture, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &offset, const Float4 &cubeArrayCoord, const Int4 &sampleId, Float &lod, bool secondLOD, SamplerFunction function);
	Vector4f sampleFloat2D(Pointer<Byte> &texture, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &offset, const Float4 &cubeArrayCoord, const Int4 &sampleId, Float &lod, bool secondLOD, SamplerFunction function);
	Vector4f sampleFloat3D(Pointer<Byte> &texture, Float4 &u, Float4 &v, Float4 &w, Vector4f &offset, const Float4 &cubeArrayCoord, const Int4 &sampleId, Float &lod, bool secondLOD, SamplerFunction function);
	Float log2sqrt(Float lod);
	Float log2(Float lod);
	void computeLod(Pointer<Byte> &texture, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Float4 &u, Float4 &v, Float4 &dsx, Float4 &dsy, SamplerFunction function);
	void computeLodCube(Pointer<Byte> &texture, Float &lod, Float4 &u, Float4 &v, Float4 &w, Float4 &dsx, Float4 &dsy, Float4 &M, SamplerFunction function);
	void computeLod3D(Pointer<Byte> &texture, Float &lod, Float4 &u, Float4 &v, Float4 &w, Float4 &dsx, Float4 &dsy, SamplerFunction function);
	Int4 cubeFace(Float4 &U, Float4 &V, Float4 &x, Float4 &y, Float4 &z, Float4 &M);
	Short4 applyOffset(Short4 &uvw, Float4 &offset, const Int4 &whd, AddressingMode mode);
	void computeIndices(UInt index[4], Short4 uuuu, Short4 vvvv, Short4 wwww, Vector4f &offset, const Pointer<Byte> &mipmap, const Short4 &cubeArrayId, const Int4 &sampleId, SamplerFunction function);
	void computeIndices(UInt index[4], Int4 uuuu, Int4 vvvv, Int4 wwww, Int4 valid, const Pointer<Byte> &mipmap, const Int4 &cubeArrayId, const Int4 &sampleId, SamplerFunction function);
	Vector4s sampleTexel(Short4 &u, Short4 &v, Short4 &s, Vector4f &offset, Pointer<Byte> &mipmap, const Short4 &cubeArrayId, const Int4 &sampleId, Pointer<Byte> buffer, SamplerFunction function);
	Vector4s sampleTexel(UInt index[4], Pointer<Byte> buffer);
	Vector4f sampleTexel(Int4 &u, Int4 &v, Int4 &s, Float4 &z, Pointer<Byte> &mipmap, const Int4 &cubeArrayId, const Int4 &sampleId, Pointer<Byte> buffer, SamplerFunction function);
	Vector4f replaceBorderTexel(const Vector4f &c, Int4 valid);
	void selectMipmap(const Pointer<Byte> &texture, Pointer<Byte> &mipmap, Pointer<Byte> &buffer, const Float &lod, bool secondLOD);
	Short4 address(const Float4 &uw, AddressingMode addressingMode, Pointer<Byte> &mipmap);
	void address(const Float4 &uw, Int4 &xyz0, Int4 &xyz1, Float4 &f, Pointer<Byte> &mipmap, Float4 &texOffset, Int4 &filter, int whd, AddressingMode addressingMode, SamplerFunction function);
	Int4 computeFilterOffset(Float &lod);

	void convertSigned15(Float4 &cf, Short4 &ci);
	void convertUnsigned16(Float4 &cf, Short4 &ci);
	void sRGBtoLinearFF00(Short4 &c);

	bool hasFloatTexture() const;
	bool hasUnnormalizedIntegerTexture() const;
	bool hasUnsignedTextureComponent(int component) const;
	int textureComponentCount() const;
	bool hasThirdCoordinate() const;
	bool has16bitTextureFormat() const;
	bool has8bitTextureComponents() const;
	bool has16bitTextureComponents() const;
	bool has32bitIntegerTextureComponents() const;
	bool isYcbcrFormat() const;
	bool isRGBComponent(int component) const;
	bool borderModeActive() const;
	bool isCube() const;
	VkComponentSwizzle gatherSwizzle() const;

	Pointer<Byte> &constants;
	const Sampler &state;
};

}  // namespace sw

#ifdef ENABLE_RR_PRINT
namespace rr {

template<>
struct PrintValue::Ty<sw::SamplerFunction>
{
	static std::string fmt(const sw::SamplerFunction &v)
	{
		return std::string("SamplerFunction[") +
		       "method: " + std::to_string(v.method) +
		       ", offset: " + std::to_string(v.offset) +
		       ", sample: " + std::to_string(v.sample) +
		       "]";
	}

	static std::vector<rr::Value *> val(const sw::SamplerFunction &v) { return {}; }
};

}  // namespace rr
#endif  // ENABLE_RR_PRINT

#endif  // sw_SamplerCore_hpp
