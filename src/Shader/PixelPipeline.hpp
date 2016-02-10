// SwiftShader Software Renderer
//
// Copyright(c) 2015 Google Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of Google Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef sw_PixelPipeline_hpp
#define sw_PixelPipeline_hpp

#include "PixelRoutine.hpp"

namespace sw
{
	class PixelPipeline : public PixelRoutine
	{
	public:
		PixelPipeline(const PixelProcessor::State &state, const PixelShader *shader) :
			PixelRoutine(state, shader), current(rs[0]), diffuse(vs[0]), specular(vs[1]), perturbate(false), luminance(false), previousScaling(false) {}
		virtual ~PixelPipeline() {}

	protected:
		virtual void setBuiltins(Int &x, Int &y, Float4(&z)[4], Float4 &w);
		virtual void applyShader(Int cMask[4]);
		virtual Bool alphaTest(Int cMask[4]);
		virtual void rasterOperation(Float4 &fog, Pointer<Byte> cBuffer[4], Int &x, Int sMask[4], Int zMask[4], Int cMask[4]);

	private:
		Vector4s &current;
		Vector4s &diffuse;
		Vector4s &specular;

		Vector4s rs[6];
		Vector4s vs[2];
		Vector4s ts[6];

		// bem(l) offsets and luminance
		Float4 du;
		Float4 dv;
		Short4 L;

		// texm3x3 temporaries
		Float4 u_; // FIXME
		Float4 v_; // FIXME
		Float4 w_; // FIXME
		Float4 U;  // FIXME
		Float4 V;  // FIXME
		Float4 W;  // FIXME

		void fixedFunction();
		void blendTexture(Vector4s &temp, Vector4s &texture, int stage);
		void fogBlend(Vector4s &current, Float4 &fog);
		void specularPixel(Vector4s &current, Vector4s &specular);

		void sampleTexture(Vector4s &c, int coordinates, int sampler, bool project = false);
		void sampleTexture(Vector4s &c, int sampler, Float4 &u, Float4 &v, Float4 &w, Float4 &q, bool project = false, bool bias = false);
		void sampleTexture(Vector4s &c, int sampler, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, bool project = false, bool bias = false, bool gradients = false, bool lodProvided = false);

		Short4 convertFixed12(RValue<Float4> cf);
		void convertFixed12(Vector4s &cs, Vector4f &cf);
		Float4 convertSigned12(Short4 &cs);
		void convertSigned12(Vector4f &cf, Vector4s &cs);

		void writeDestination(Vector4s &d, const Dst &dst);
		Vector4s fetchRegisterS(const Src &src);

		// Instructions
		void MOV(Vector4s &dst, Vector4s &src0);
		void ADD(Vector4s &dst, Vector4s &src0, Vector4s &src1);
		void SUB(Vector4s &dst, Vector4s &src0, Vector4s &src1);
		void MAD(Vector4s &dst, Vector4s &src0, Vector4s &src1, Vector4s &src2);
		void MUL(Vector4s &dst, Vector4s &src0, Vector4s &src1);
		void DP3(Vector4s &dst, Vector4s &src0, Vector4s &src1);
		void DP4(Vector4s &dst, Vector4s &src0, Vector4s &src1);
		void LRP(Vector4s &dst, Vector4s &src0, Vector4s &src1, Vector4s &src2);
		void TEXCOORD(Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, int coordinate);
		void TEXCRD(Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, int coordinate, bool project);
		void TEXDP3(Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, Vector4s &src);
		void TEXDP3TEX(Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4s &src0);
		void TEXKILL(Int cMask[4], Float4 &u, Float4 &v, Float4 &s);
		void TEXKILL(Int cMask[4], Vector4s &dst);
		void TEX(Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, int stage, bool project);
		void TEXLD(Vector4s &dst, Vector4s &src, int stage, bool project);
		void TEXBEM(Vector4s &dst, Vector4s &src, Float4 &u, Float4 &v, Float4 &s, int stage);
		void TEXBEML(Vector4s &dst, Vector4s &src, Float4 &u, Float4 &v, Float4 &s, int stage);
		void TEXREG2AR(Vector4s &dst, Vector4s &src0, int stage);
		void TEXREG2GB(Vector4s &dst, Vector4s &src0, int stage);
		void TEXREG2RGB(Vector4s &dst, Vector4s &src0, int stage);
		void TEXM3X2DEPTH(Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, Vector4s &src, bool signedScaling);
		void TEXM3X2PAD(Float4 &u, Float4 &v, Float4 &s, Vector4s &src0, int component, bool signedScaling);
		void TEXM3X2TEX(Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4s &src0, bool signedScaling);
		void TEXM3X3(Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, Vector4s &src0, bool signedScaling);
		void TEXM3X3PAD(Float4 &u, Float4 &v, Float4 &s, Vector4s &src0, int component, bool signedScaling);
		void TEXM3X3SPEC(Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4s &src0, Vector4s &src1);
		void TEXM3X3TEX(Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4s &src0, bool singedScaling);
		void TEXM3X3VSPEC(Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4s &src0);
		void TEXDEPTH();
		void CND(Vector4s &dst, Vector4s &src0, Vector4s &src1, Vector4s &src2);
		void CMP(Vector4s &dst, Vector4s &src0, Vector4s &src1, Vector4s &src2);
		void BEM(Vector4s &dst, Vector4s &src0, Vector4s &src1, int stage);

		bool perturbate;
		bool luminance;
		bool previousScaling;
	};
}

#endif
