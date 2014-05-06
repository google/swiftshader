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

#ifndef sw_PixelRoutine_hpp
#define sw_PixelRoutine_hpp

#include "Rasterizer.hpp"
#include "ShaderCore.hpp"

#include "Types.hpp"

namespace sw
{
	extern bool forceClearRegisters;

	class PixelShader;
	class SamplerCore;

	class PixelRoutine : public Rasterizer, public ShaderCore
	{
		friend PixelProcessor;   // FIXME

	public:
		PixelRoutine(const PixelProcessor::State &state, const PixelShader *pixelShader);

		~PixelRoutine();

	protected:
		struct Registers
		{
			Registers() : current(ri[0]), diffuse(vi[0]), specular(vi[1]), callStack(4), aL(4), increment(4), iteration(4), enableStack(1 + 24), vx(10), vy(10), vz(10), vw(10)
			{
				if(forceClearRegisters)
				{
					for(int i = 0; i < 10; i++)
					{
						vx[i] = Float4(0, 0, 0, 0);
						vy[i] = Float4(0, 0, 0, 0);
						vz[i] = Float4(0, 0, 0, 0);
						vw[i] = Float4(0, 0, 0, 0);
					}

					for(int i = 0; i < 4; i++)
					{
						oC[i].r = Float4(0.0f);
						oC[i].g = Float4(0.0f);
						oC[i].b = Float4(0.0f);
						oC[i].a = Float4(0.0f);
					}
				}

				loopDepth = -1;
				enableStack[0] = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
				enableBreak = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);

				occlusion = 0;
				
				#if PERF_PROFILE
					for(int i = 0; i < PERF_TIMERS; i++)
					{
						cycles[i] = 0;
					}
				#endif
			}

			Pointer<Byte> constants;

			Pointer<Byte> primitive;
			Int cluster;
			Pointer<Byte> data;

			Float4 z[4];
			Float4 rhw;

			Float4 Dz[4];
			Float4 Dw;
			Float4 Dv[10][4];
			Float4 Df;

			Color4i &current;
			Color4i &diffuse;
			Color4i &specular;

			Color4i ri[6];
			Color4i vi[2];
			Color4i ti[6];

			Color4f rf[32];
			Array<Float4> vx;
			Array<Float4> vy;
			Array<Float4> vz;
			Array<Float4> vw;

			Color4f vPos;
			Color4f vFace;

			Color4f oC[4];
			Float4 oDepth;

			Color4f p0;
			Array<Int> aL;

			Array<Int> increment;
			Array<Int> iteration;

			Int loopDepth;
			Int stackIndex;   // FIXME: Inc/decrement callStack
			Array<UInt> callStack;

			Int enableIndex;
			Array<Int4> enableStack;
			Int4 enableBreak;

			// bem(l) offsets and luminance
			Float4 du;
			Float4 dv;
			Short4 L;

			// texm3x3 temporaries
			Float4 u_;   // FIXME
			Float4 v_;   // FIXME
			Float4 w_;   // FIXME
			Float4 U;   // FIXME
			Float4 V;   // FIXME
			Float4 W;   // FIXME

			UInt occlusion;

			#if PERF_PROFILE
				Long cycles[PERF_TIMERS];
			#endif
		};

		void quad(Registers &r, Pointer<Byte> cBuffer[4], Pointer<Byte> &zBuffer, Pointer<Byte> &sBuffer, Int cMask[4], Int &x, Int &y);

		Float4 interpolate(Float4 &x, Float4 &D, Float4 &rhw, Pointer<Byte> planeEquation, bool flat, bool perspective);
		Float4 interpolateCentroid(Float4 &x, Float4 &y, Float4 &rhw, Pointer<Byte> planeEquation, bool flat, bool perspective);
		void stencilTest(Registers &r, Pointer<Byte> &sBuffer, int q, Int &x, Int &sMask, Int &cMask);
		void stencilTest(Registers &r, Byte8 &value, Context::StencilCompareMode stencilCompareMode, bool CCW);
		void stencilOperation(Registers &r, Byte8 &newValue, Byte8 &bufferValue, Context::StencilOperation stencilPassOperation, Context::StencilOperation stencilZFailOperation, Context::StencilOperation stencilFailOperation, bool CCW, Int &zMask, Int &sMask);
		void stencilOperation(Registers &r, Byte8 &output, Byte8 &bufferValue, Context::StencilOperation operation, bool CCW);
		Bool depthTest(Registers &r, Pointer<Byte> &zBuffer, int q, Int &x, Float4 &z, Int &sMask, Int &zMask, Int &cMask);
		void blendTexture(Registers &r, Color4i &current, Color4i &temp, Color4i &texture, int stage);
		void alphaTest(Registers &r, Int &aMask, Short4 &alpha);
		void alphaToCoverage(Registers &r, Int cMask[4], Float4 &alpha);
		Bool alphaTest(Registers &r, Int cMask[4], Color4i &current);
		Bool alphaTest(Registers &r, Int cMask[4], Color4f &c0);
		void fogBlend(Registers &r, Color4i &current, Float4 &fog, Float4 &z, Float4 &rhw);
		void fogBlend(Registers &r, Color4f &c0, Float4 &fog, Float4 &z, Float4 &rhw);
		void pixelFog(Registers &r, Float4 &visibility, Float4 &z, Float4 &rhw);
		void specularPixel(Color4i &current, Color4i &specular);

		void sampleTexture(Registers &r, Color4i &c, int coordinates, int sampler, bool project = false);
		void sampleTexture(Registers &r, Color4i &c, int sampler, Float4 &u, Float4 &v, Float4 &w, Float4 &q, bool project = false, bool bias = false, bool fixed12 = true);
		void sampleTexture(Registers &r, Color4i &c, int sampler, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Color4f &dsx, Color4f &dsy, bool project = false, bool bias = false, bool fixed12 = true, bool gradients = false, bool lodProvided = false);
		void sampleTexture(Registers &r, Color4f &c, int sampler, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Color4f &dsx, Color4f &dsy, bool project = false, bool bias = false, bool gradients = false, bool lodProvided = false);
	
		// Raster operations
		void clampColor(Color4f oC[4]);
		void rasterOperation(Color4i &current, Registers &r, Float4 &fog, Pointer<Byte> &cBuffer, Int &x, Int sMask[4], Int zMask[4], Int cMask[4]);
		void rasterOperation(Color4f oC[4], Registers &r, Float4 &fog, Pointer<Byte> cBuffer[4], Int &x, Int sMask[4], Int zMask[4], Int cMask[4]);
		void blendFactor(Registers &r, const Color4i &blendFactor, const Color4i &current, const Color4i &pixel, Context::BlendFactor blendFactorActive);
		void blendFactorAlpha(Registers &r, const Color4i &blendFactor, const Color4i &current, const Color4i &pixel, Context::BlendFactor blendFactorAlphaActive);
		void alphaBlend(Registers &r, int index, Pointer<Byte> &cBuffer, Color4i &current, Int &x);
		void writeColor(Registers &r, int index, Pointer<Byte> &cBuffer, Int &i, Color4i &current, Int &sMask, Int &zMask, Int &cMask);
		void blendFactor(Registers &r, const Color4f &blendFactor, const Color4f &oC, const Color4f &pixel, Context::BlendFactor blendFactorActive);
		void blendFactorAlpha(Registers &r, const Color4f &blendFactor, const Color4f &oC, const Color4f &pixel, Context::BlendFactor blendFactorAlphaActive);
		void alphaBlend(Registers &r, int index, Pointer<Byte> &cBuffer, Color4f &oC, Int &x);
		void writeColor(Registers &r, int index, Pointer<Byte> &cBuffer, Int &i, Color4f &oC, Int &sMask, Int &zMask, Int &cMask);
		void writeStencil(Registers &r, Pointer<Byte> &sBuffer, int q, Int &x, Int &sMask, Int &zMask, Int &cMask);
		void writeDepth(Registers &r, Pointer<Byte> &zBuffer, int q, Int &x, Float4 &z, Int &zMask);

		void ps_1_x(Registers &r, Int cMask[4]);
		void ps_2_x(Registers &r, Int cMask[4]);

		Short4 convertFixed12(Float4 &cf);
		void convertFixed12(Color4i &ci, Color4f &cf);
		Float4 convertSigned12(Short4 &ci);
		void convertSigned12(Color4f &cf, Color4i &ci);
		Float4 convertUnsigned16(UShort4 ci);
		UShort4 convertFixed16(Float4 &cf, bool saturate = true);
		void convertFixed16(Color4i &ci, Color4f &cf, bool saturate = true);
		void sRGBtoLinear16_16(Registers &r, Color4i &c);
		void sRGBtoLinear12_16(Registers &r, Color4i &c);
		void linearToSRGB16_16(Registers &r, Color4i &c);
		void linearToSRGB12_16(Registers &r, Color4i &c);
		Float4 sRGBtoLinear(const Float4 &x);
		Float4 linearToSRGB(const Float4 &x);

		typedef Shader::Instruction::DestinationParameter Dst;
		typedef Shader::Instruction::SourceParameter Src;
		typedef Shader::Instruction::Operation Op;
		typedef Shader::Instruction::Operation::Control Control;

		// ps_1_x instructions
		void MOV(Color4i &dst, Color4i &src0);
		void ADD(Color4i &dst, Color4i &src0, Color4i &src1);
		void SUB(Color4i &dst, Color4i &src0, Color4i &src1);
		void MAD(Color4i &dst, Color4i &src0, Color4i &src1, Color4i &src2);
		void MUL(Color4i &dst, Color4i &src0, Color4i &src1);
		void DP3(Color4i &dst, Color4i &src0, Color4i &src1);
		void DP4(Color4i &dst, Color4i &src0, Color4i &src1);
		void LRP(Color4i &dst, Color4i &src0, Color4i &src1, Color4i &src2);
		void TEXCOORD(Color4i &dst, Float4 &u, Float4 &v, Float4 &s, int coordinate);
		void TEXCRD(Color4i &dst, Float4 &u, Float4 &v, Float4 &s, int coordinate, bool project);
		void TEXDP3(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, Color4i &src);
		void TEXDP3TEX(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Color4i &src0);
		void TEXKILL(Int cMask[4], Float4 &u, Float4 &v, Float4 &s);
		void TEXKILL(Int cMask[4], Color4i &dst);
		void TEX(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, bool project);
		void TEXLD(Registers &r, Color4i &dst, Color4i &src, int stage, bool project);
		void TEXBEM(Registers &r, Color4i &dst, Color4i &src, Float4 &u, Float4 &v, Float4 &s, int stage);
		void TEXBEML(Registers &r, Color4i &dst, Color4i &src, Float4 &u, Float4 &v, Float4 &s, int stage);
		void TEXREG2AR(Registers &r, Color4i &dst, Color4i &src0, int stage);
		void TEXREG2GB(Registers &r, Color4i &dst, Color4i &src0, int stage);
		void TEXREG2RGB(Registers &r, Color4i &dst, Color4i &src0, int stage);
		void TEXM3X2DEPTH(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, Color4i &src, bool signedScaling);
		void TEXM3X2PAD(Registers &r, Float4 &u, Float4 &v, Float4 &s, Color4i &src0, int component, bool signedScaling);
		void TEXM3X2TEX(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Color4i &src0, bool signedScaling);
		void TEXM3X3(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, Color4i &src0, bool signedScaling);
		void TEXM3X3PAD(Registers &r, Float4 &u, Float4 &v, Float4 &s, Color4i &src0, int component, bool signedScaling);
		void TEXM3X3SPEC(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Color4i &src0, Color4i &src1);
		void TEXM3X3TEX(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Color4i &src0, bool singedScaling);
		void TEXM3X3VSPEC(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Color4i &src0);
		void TEXDEPTH(Registers &r);
		void CND(Color4i &dst, Color4i &src0, Color4i &src1, Color4i &src2);
		void CMP(Color4i &dst, Color4i &src0, Color4i &src1, Color4i &src2);
		void BEM(Registers &r, Color4i &dst, Color4i &src0, Color4i &src1, int stage);

		// ps_2_x instructions
		void M3X2(Registers &r, Color4f &dst, Color4f &src0, const Src &src1);
		void M3X3(Registers &r, Color4f &dst, Color4f &src0, const Src &src1);
		void M3X4(Registers &r, Color4f &dst, Color4f &src0, const Src &src1);
		void M4X3(Registers &r, Color4f &dst, Color4f &src0, const Src &src1);
		void M4X4(Registers &r, Color4f &dst, Color4f &src0, const Src &src1);
		void TEXLD(Registers &r, Color4f &dst, Color4f &src0, const Src &src1, bool project, bool bias);
		void TEXLDD(Registers &r, Color4f &dst, Color4f &src0, const Src &src1, Color4f &src2,  Color4f &src3, bool project, bool bias);
		void TEXLDL(Registers &r, Color4f &dst, Color4f &src0, const Src &src1, bool project, bool bias);
		void TEXKILL(Int cMask[4], Color4f &src, unsigned char mask);
		void DSX(Color4f &dst, Color4f &src);
		void DSY(Color4f &dst, Color4f &src);
		void BREAK(Registers &r);
		void BREAKC(Registers &r, Color4f &src0, Color4f &src1, Control);
		void BREAKP(Registers &r, const Src &predicateRegister);
		void CALL(Registers &r, int labelIndex);
		void CALLNZ(Registers &r, int labelIndex, const Src &src);
		void CALLNZb(Registers &r, int labelIndex, const Src &boolRegister);
		void CALLNZp(Registers &r, int labelIndex, const Src &predicateRegister);
		void ELSE(Registers &r);
		void ENDIF(Registers &r);
		void ENDLOOP(Registers &r);
		void ENDREP(Registers &r);
		void IF(Registers &r, const Src &src);
		void IFb(Registers &r, const Src &boolRegister);
		void IFp(Registers &r, const Src &predicateRegister);
		void IFC(Registers &r, Color4f &src0, Color4f &src1, Control);
		void LABEL(int labelIndex);
		void LOOP(Registers &r, const Src &integerRegister);
		void REP(Registers &r, const Src &integerRegister);
		void RET(Registers &r);

		void readConstant(Registers &r, int index);

		void writeDestination(Registers &r, Color4i &d, const Dst &dst);
		Color4i regi(Registers &r, const Src &src);
		Color4f reg(Registers &r, const Src &src, int offset = 0);

		bool colorUsed();
		unsigned short pixelShaderVersion() const;

	private:
		SamplerCore *sampler[16];

		bool perturbate;
		bool luminance;
		bool previousScaling;

		bool returns;
		int ifDepth;
		int loopRepDepth;
		int breakDepth;

		// FIXME: Get rid of llvm::
		llvm::BasicBlock *ifFalseBlock[24 + 24];
		llvm::BasicBlock *loopRepTestBlock[4];
		llvm::BasicBlock *loopRepEndBlock[4];
		llvm::BasicBlock *labelBlock[2048];
		std::vector<llvm::BasicBlock*> callRetBlock;
		llvm::BasicBlock *returnBlock;
		bool isConditionalIf[24 + 24];

		const PixelShader *const pixelShader;
	};
}

#endif   // sw_PixelRoutine_hpp
