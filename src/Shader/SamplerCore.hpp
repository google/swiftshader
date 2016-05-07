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
#include "Reactor/Nucleus.hpp"

namespace sw
{
	enum SamplerMethod
	{
		Implicit,
		Bias,
		Lod,
		Grad,
	};

	class SamplerCore
	{
	public:
		SamplerCore(Pointer<Byte> &r, const Sampler::State &state);

		void sampleTexture(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, SamplerMethod method = Implicit);
		void sampleTexture(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, SamplerMethod method = Implicit);

	private:
		void sampleTexture(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, SamplerMethod method, bool fixed12);

		void border(Short4 &mask, Float4 &coordinates);
		void border(Int4 &mask, Float4 &coordinates);
		Short4 offsetSample(Short4 &uvw, Pointer<Byte> &mipmap, int halfOffset, bool wrap, int count, Float &lod);
		void sampleFilter(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Int face[4], SamplerMethod method);
		void sampleAniso(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Int face[4], bool secondLOD, SamplerMethod method);
		void sampleQuad(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Int face[4], bool secondLOD);
		void sampleQuad2D(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Int face[4], bool secondLOD);
		void sample3D(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, bool secondLOD);
		void sampleFloatFilter(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Int face[4], SamplerMethod method);
		void sampleFloatAniso(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Int face[4], bool secondLOD, SamplerMethod method);
		void sampleFloat(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Int face[4], bool secondLOD);
		void sampleFloat2D(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Int face[4], bool secondLOD);
		void sampleFloat3D(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, bool secondLOD);
		void computeLod(Pointer<Byte> &texture, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Float4 &u, Float4 &v, const Float &lodBias, Vector4f &dsx, Vector4f &dsy, SamplerMethod method);
		void computeLodCube(Pointer<Byte> &texture, Float &lod, Float4 &x, Float4 &y, Float4 &z, const Float &lodBias, Vector4f &dsx, Vector4f &dsy, SamplerMethod method);
		void computeLod3D(Pointer<Byte> &texture, Float &lod, Float4 &u, Float4 &v, Float4 &w, const Float &lodBias, Vector4f &dsx, Vector4f &dsy, SamplerMethod method);
		void cubeFace(Int face[4], Float4 &U, Float4 &V, Float4 &lodX, Float4 &lodY, Float4 &lodZ, Float4 &x, Float4 &y, Float4 &z);
		void computeIndices(Int index[4], Short4 uuuu, Short4 vvvv, Short4 wwww, const Pointer<Byte> &mipmap);
		void sampleTexel(Vector4s &c, Short4 &u, Short4 &v, Short4 &s, Pointer<Byte> &mipmap, Pointer<Byte> buffer[4]);
		void sampleTexel(Vector4f &c, Short4 &u, Short4 &v, Short4 &s, Float4 &z, Pointer<Byte> &mipmap, Pointer<Byte> buffer[4]);
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
