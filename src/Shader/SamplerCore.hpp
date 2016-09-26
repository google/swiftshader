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

#include "PixelRoutine.hpp"
#include "Reactor/Reactor.hpp"

namespace sw
{
	enum SamplerMethod
	{
		Implicit,
		Bias,
		Lod,
		Grad,
		Fetch
	};

	enum SamplerOption
	{
		None,
		Offset
	};

	struct SamplerFunction
	{
		SamplerFunction(SamplerMethod method, SamplerOption option = None) : method(method), option(option) {}
		operator SamplerMethod() { return method; }

		const SamplerMethod method;
		const SamplerOption option;
 	};

	class SamplerCore
	{
	public:
		SamplerCore(Pointer<Byte> &r, const Sampler::State &state);

		void sampleTexture(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy);
		void sampleTexture(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, Vector4f &offset, SamplerFunction function);
		void textureSize(Pointer<Byte> &mipmap, Vector4f &size, Float4 &lod);

	private:
		void sampleTexture(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, Vector4f &offset, SamplerFunction function, bool fixed12);

		void border(Short4 &mask, Float4 &coordinates);
		void border(Int4 &mask, Float4 &coordinates);
		Short4 offsetSample(Short4 &uvw, Pointer<Byte> &mipmap, int halfOffset, bool wrap, int count, Float &lod);
		void sampleFilter(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Vector4f &offset, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Int face[4], SamplerFunction function);
		void sampleAniso(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Vector4f &offset, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Int face[4], bool secondLOD, SamplerFunction function);
		void sampleQuad(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Vector4f &offset, Float &lod, Int face[4], bool secondLOD, SamplerFunction function);
		void sampleQuad2D(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Vector4f &offset, Float &lod, Int face[4], bool secondLOD, SamplerFunction function);
		void sample3D(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Vector4f &offset, Float &lod, bool secondLOD, SamplerFunction function);
		void sampleFloatFilter(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Vector4f &offset, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Int face[4], SamplerFunction function);
		void sampleFloatAniso(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Vector4f &offset, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Int face[4], bool secondLOD, SamplerFunction function);
		void sampleFloat(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Vector4f &offset, Float &lod, Int face[4], bool secondLOD, SamplerFunction function);
		void sampleFloat2D(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Vector4f &offset, Float &lod, Int face[4], bool secondLOD, SamplerFunction function);
		void sampleFloat3D(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Vector4f &offset, Float &lod, bool secondLOD, SamplerFunction function);
		void computeLod(Pointer<Byte> &texture, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Float4 &u, Float4 &v, const Float &lodBias, Vector4f &dsx, Vector4f &dsy, SamplerFunction function);
		void computeLodCube(Pointer<Byte> &texture, Float &lod, Float4 &x, Float4 &y, Float4 &z, const Float &lodBias, Vector4f &dsx, Vector4f &dsy, SamplerFunction function);
		void computeLod3D(Pointer<Byte> &texture, Float &lod, Float4 &u, Float4 &v, Float4 &w, const Float &lodBias, Vector4f &dsx, Vector4f &dsy, SamplerFunction function);
		void cubeFace(Int face[4], Float4 &U, Float4 &V, Float4 &lodX, Float4 &lodY, Float4 &lodZ, Float4 &x, Float4 &y, Float4 &z);
		Short4 applyOffset(Short4 &uvw, Float4 &offset, const Int4 &whd, AddressingMode mode);
		void computeIndices(Int index[4], Short4 uuuu, Short4 vvvv, Short4 wwww, Vector4f &offset, const Pointer<Byte> &mipmap, SamplerFunction function);
		void sampleTexel(Vector4s &c, Short4 &u, Short4 &v, Short4 &s, Vector4f &offset, Pointer<Byte> &mipmap, Pointer<Byte> buffer[4], SamplerFunction function);
		void sampleTexel(Vector4f &c, Short4 &u, Short4 &v, Short4 &s, Vector4f &offset, Float4 &z, Pointer<Byte> &mipmap, Pointer<Byte> buffer[4], SamplerFunction function);
		void selectMipmap(Pointer<Byte> &texture, Pointer<Byte> buffer[4], Pointer<Byte> &mipmap, Float &lod, Int face[4], bool secondLOD);
		Short4 address(Float4 &uw, AddressingMode addressingMode, Pointer<Byte>& mipmap);

		void convertFixed12(Short4 &ci, Float4 &cf);
		void convertFixed12(Vector4s &cs, Vector4f &cf);
		void convertSigned12(Float4 &cf, Short4 &ci);
		void convertSigned15(Float4 &cf, Short4 &ci);
		void convertUnsigned16(Float4 &cf, Short4 &ci);
		void sRGBtoLinear16_8_12(Short4 &c);
		void sRGBtoLinear16_6_12(Short4 &c);
		void sRGBtoLinear16_5_12(Short4 &c);

		bool hasFloatTexture() const;
		bool hasUnsignedTextureComponent(int component) const;
		int textureComponentCount() const;
		bool has16bitTextureFormat() const;
		bool has8bitTextureComponents() const;
		bool has16bitTextureComponents() const;
		bool hasYuvFormat() const;
		bool isRGBComponent(int component) const;

		Pointer<Byte> &constants;
		const Sampler::State &state;
	};
}

#endif   // sw_SamplerCore_hpp
