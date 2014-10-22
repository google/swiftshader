// SwiftShader Software Renderer
//
// Copyright(c) 2005-2013 TransGaming Inc.
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
#include "PixelShader.hpp"

#include "Types.hpp"

namespace sw
{
	extern bool forceClearRegisters;

	class PixelShader;
	class SamplerCore;

	class PixelRoutine : public Rasterizer, public ShaderCore
	{
		friend class PixelProcessor;   // FIXME

	public:
		PixelRoutine(const PixelProcessor::State &state, const PixelShader *shader);

		~PixelRoutine();

	protected:
		struct Registers
		{
			Registers(const PixelShader *shader) :
				current(ri[0]), diffuse(vi[0]), specular(vi[1]),
				rf(shader && shader->dynamicallyIndexedTemporaries),
				vf(shader && shader->dynamicallyIndexedInput)
			{
				if(!shader || shader->getVersion() < 0x0200 || forceClearRegisters)
				{
					for(int i = 0; i < 10; i++)
					{
						vf[i].x = Float4(0.0f);
						vf[i].y = Float4(0.0f);
						vf[i].z = Float4(0.0f);
						vf[i].w = Float4(0.0f);
					}
				}

				loopDepth = -1;
				enableStack[0] = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
				
				if(shader && shader->containsBreakInstruction())
				{
					enableBreak = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
				}

				if(shader && shader->containsContinueInstruction())
				{
					enableContinue = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
				}

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
			Float4 w;
			Float4 rhw;

			Float4 Dz[4];
			Float4 Dw;
			Float4 Dv[10][4];
			Float4 Df;

			Vector4i &current;
			Vector4i &diffuse;
			Vector4i &specular;

			Vector4i ri[6];
			Vector4i vi[2];
			Vector4i ti[6];

			RegisterArray<4096> rf;
			RegisterArray<10> vf;

			Vector4f vPos;
			Vector4f vFace;

			Vector4f oC[4];
			Float4 oDepth;

			Vector4f p0;
			Array<Int, 4> aL;

			Array<Int, 4> increment;
			Array<Int, 4> iteration;

			Int loopDepth;
			Int stackIndex;   // FIXME: Inc/decrement callStack
			Array<UInt, 16> callStack;

			Int enableIndex;
			Array<Int4, 1 + 24> enableStack;
			Int4 enableBreak;
			Int4 enableContinue;
			Int4 enableLeave;

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

		typedef Shader::DestinationParameter Dst;
		typedef Shader::SourceParameter Src;
		typedef Shader::Control Control;

		void quad(Registers &r, Pointer<Byte> cBuffer[4], Pointer<Byte> &zBuffer, Pointer<Byte> &sBuffer, Int cMask[4], Int &x, Int &y);

		Float4 interpolate(Float4 &x, Float4 &D, Float4 &rhw, Pointer<Byte> planeEquation, bool flat, bool perspective);
		Float4 interpolateCentroid(Float4 &x, Float4 &y, Float4 &rhw, Pointer<Byte> planeEquation, bool flat, bool perspective);
		void stencilTest(Registers &r, Pointer<Byte> &sBuffer, int q, Int &x, Int &sMask, Int &cMask);
		void stencilTest(Registers &r, Byte8 &value, StencilCompareMode stencilCompareMode, bool CCW);
		void stencilOperation(Registers &r, Byte8 &newValue, Byte8 &bufferValue, StencilOperation stencilPassOperation, StencilOperation stencilZFailOperation, StencilOperation stencilFailOperation, bool CCW, Int &zMask, Int &sMask);
		void stencilOperation(Registers &r, Byte8 &output, Byte8 &bufferValue, StencilOperation operation, bool CCW);
		Bool depthTest(Registers &r, Pointer<Byte> &zBuffer, int q, Int &x, Float4 &z, Int &sMask, Int &zMask, Int &cMask);
		void blendTexture(Registers &r, Vector4i &temp, Vector4i &texture, int stage);
		void alphaTest(Registers &r, Int &aMask, Short4 &alpha);
		void alphaToCoverage(Registers &r, Int cMask[4], Float4 &alpha);
		Bool alphaTest(Registers &r, Int cMask[4], Vector4i &current);
		Bool alphaTest(Registers &r, Int cMask[4], Vector4f &c0);
		void fogBlend(Registers &r, Vector4i &current, Float4 &fog, Float4 &z, Float4 &rhw);
		void fogBlend(Registers &r, Vector4f &c0, Float4 &fog, Float4 &z, Float4 &rhw);
		void pixelFog(Registers &r, Float4 &visibility, Float4 &z, Float4 &rhw);
		void specularPixel(Vector4i &current, Vector4i &specular);

		void sampleTexture(Registers &r, Vector4i &c, int coordinates, int sampler, bool project = false);
		void sampleTexture(Registers &r, Vector4i &c, int sampler, Float4 &u, Float4 &v, Float4 &w, Float4 &q, bool project = false, bool bias = false, bool fixed12 = true);
		void sampleTexture(Registers &r, Vector4i &c, int sampler, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, bool project = false, bool bias = false, bool fixed12 = true, bool gradients = false, bool lodProvided = false);
		void sampleTexture(Registers &r, Vector4f &c, const Src &sampler, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, bool project = false, bool bias = false, bool gradients = false, bool lodProvided = false);
		void sampleTexture(Registers &r, Vector4f &c, int sampler, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, bool project = false, bool bias = false, bool gradients = false, bool lodProvided = false);
	
		// Raster operations
		void clampColor(Vector4f oC[4]);
		void rasterOperation(Vector4i &current, Registers &r, Float4 &fog, Pointer<Byte> &cBuffer, Int &x, Int sMask[4], Int zMask[4], Int cMask[4]);
		void rasterOperation(Vector4f oC[4], Registers &r, Float4 &fog, Pointer<Byte> cBuffer[4], Int &x, Int sMask[4], Int zMask[4], Int cMask[4]);
		void blendFactor(Registers &r, const Vector4i &blendFactor, const Vector4i &current, const Vector4i &pixel, BlendFactor blendFactorActive);
		void blendFactorAlpha(Registers &r, const Vector4i &blendFactor, const Vector4i &current, const Vector4i &pixel, BlendFactor blendFactorAlphaActive);
		void alphaBlend(Registers &r, int index, Pointer<Byte> &cBuffer, Vector4i &current, Int &x);
		void writeColor(Registers &r, int index, Pointer<Byte> &cBuffer, Int &i, Vector4i &current, Int &sMask, Int &zMask, Int &cMask);
		void blendFactor(Registers &r, const Vector4f &blendFactor, const Vector4f &oC, const Vector4f &pixel, BlendFactor blendFactorActive);
		void blendFactorAlpha(Registers &r, const Vector4f &blendFactor, const Vector4f &oC, const Vector4f &pixel, BlendFactor blendFactorAlphaActive);
		void alphaBlend(Registers &r, int index, Pointer<Byte> &cBuffer, Vector4f &oC, Int &x);
		void writeColor(Registers &r, int index, Pointer<Byte> &cBuffer, Int &i, Vector4f &oC, Int &sMask, Int &zMask, Int &cMask);
		void writeStencil(Registers &r, Pointer<Byte> &sBuffer, int q, Int &x, Int &sMask, Int &zMask, Int &cMask);
		void writeDepth(Registers &r, Pointer<Byte> &zBuffer, int q, Int &x, Float4 &z, Int &zMask);

		void ps_1_x(Registers &r, Int cMask[4]);
		void ps_2_x(Registers &r, Int cMask[4]);

		Short4 convertFixed12(RValue<Float4> cf);
		void convertFixed12(Vector4i &ci, Vector4f &cf);
		Float4 convertSigned12(Short4 &ci);
		void convertSigned12(Vector4f &cf, Vector4i &ci);
		Float4 convertUnsigned16(UShort4 ci);
		UShort4 convertFixed16(Float4 &cf, bool saturate = true);
		void convertFixed16(Vector4i &ci, Vector4f &cf, bool saturate = true);
		void sRGBtoLinear16_16(Registers &r, Vector4i &c);
		void sRGBtoLinear12_16(Registers &r, Vector4i &c);
		void linearToSRGB16_16(Registers &r, Vector4i &c);
		void linearToSRGB12_16(Registers &r, Vector4i &c);
		Float4 sRGBtoLinear(const Float4 &x);
		Float4 linearToSRGB(const Float4 &x);

		// ps_1_x instructions
		void MOV(Vector4i &dst, Vector4i &src0);
		void ADD(Vector4i &dst, Vector4i &src0, Vector4i &src1);
		void SUB(Vector4i &dst, Vector4i &src0, Vector4i &src1);
		void MAD(Vector4i &dst, Vector4i &src0, Vector4i &src1, Vector4i &src2);
		void MUL(Vector4i &dst, Vector4i &src0, Vector4i &src1);
		void DP3(Vector4i &dst, Vector4i &src0, Vector4i &src1);
		void DP4(Vector4i &dst, Vector4i &src0, Vector4i &src1);
		void LRP(Vector4i &dst, Vector4i &src0, Vector4i &src1, Vector4i &src2);
		void TEXCOORD(Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, int coordinate);
		void TEXCRD(Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, int coordinate, bool project);
		void TEXDP3(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, Vector4i &src);
		void TEXDP3TEX(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4i &src0);
		void TEXKILL(Int cMask[4], Float4 &u, Float4 &v, Float4 &s);
		void TEXKILL(Int cMask[4], Vector4i &dst);
		void TEX(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, bool project);
		void TEXLD(Registers &r, Vector4i &dst, Vector4i &src, int stage, bool project);
		void TEXBEM(Registers &r, Vector4i &dst, Vector4i &src, Float4 &u, Float4 &v, Float4 &s, int stage);
		void TEXBEML(Registers &r, Vector4i &dst, Vector4i &src, Float4 &u, Float4 &v, Float4 &s, int stage);
		void TEXREG2AR(Registers &r, Vector4i &dst, Vector4i &src0, int stage);
		void TEXREG2GB(Registers &r, Vector4i &dst, Vector4i &src0, int stage);
		void TEXREG2RGB(Registers &r, Vector4i &dst, Vector4i &src0, int stage);
		void TEXM3X2DEPTH(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, Vector4i &src, bool signedScaling);
		void TEXM3X2PAD(Registers &r, Float4 &u, Float4 &v, Float4 &s, Vector4i &src0, int component, bool signedScaling);
		void TEXM3X2TEX(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4i &src0, bool signedScaling);
		void TEXM3X3(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, Vector4i &src0, bool signedScaling);
		void TEXM3X3PAD(Registers &r, Float4 &u, Float4 &v, Float4 &s, Vector4i &src0, int component, bool signedScaling);
		void TEXM3X3SPEC(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4i &src0, Vector4i &src1);
		void TEXM3X3TEX(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4i &src0, bool singedScaling);
		void TEXM3X3VSPEC(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4i &src0);
		void TEXDEPTH(Registers &r);
		void CND(Vector4i &dst, Vector4i &src0, Vector4i &src1, Vector4i &src2);
		void CMP(Vector4i &dst, Vector4i &src0, Vector4i &src1, Vector4i &src2);
		void BEM(Registers &r, Vector4i &dst, Vector4i &src0, Vector4i &src1, int stage);

		// ps_2_x instructions
		void M3X2(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1);
		void M3X3(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1);
		void M3X4(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1);
		void M4X3(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1);
		void M4X4(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1);
		void TEXLD(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1, bool project, bool bias);
		void TEXLDD(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1, Vector4f &src2,  Vector4f &src3, bool project, bool bias);
		void TEXLDL(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1, bool project, bool bias);
		void TEXKILL(Int cMask[4], Vector4f &src, unsigned char mask);
		void DISCARD(Registers &r, Int cMask[4], const Shader::Instruction *instruction);
		void DFDX(Vector4f &dst, Vector4f &src);
		void DFDY(Vector4f &dst, Vector4f &src);
		void FWIDTH(Vector4f &dst, Vector4f &src);
		void BREAK(Registers &r);
		void BREAKC(Registers &r, Vector4f &src0, Vector4f &src1, Control);
		void BREAKP(Registers &r, const Src &predicateRegister);
		void BREAK(Registers &r, Int4 &condition);
		void CONTINUE(Registers &r);
		void TEST();
		void CALL(Registers &r, int labelIndex, int callSiteIndex);
		void CALLNZ(Registers &r, int labelIndex, int callSiteIndex, const Src &src);
		void CALLNZb(Registers &r, int labelIndex, int callSiteIndex, const Src &boolRegister);
		void CALLNZp(Registers &r, int labelIndex, int callSiteIndex, const Src &predicateRegister);
		void ELSE(Registers &r);
		void ENDIF(Registers &r);
		void ENDLOOP(Registers &r);
		void ENDREP(Registers &r);
		void ENDWHILE(Registers &r);
		void IF(Registers &r, const Src &src);
		void IFb(Registers &r, const Src &boolRegister);
		void IFp(Registers &r, const Src &predicateRegister);
		void IFC(Registers &r, Vector4f &src0, Vector4f &src1, Control);
		void IF(Registers &r, Int4 &condition);
		void LABEL(int labelIndex);
		void LOOP(Registers &r, const Src &integerRegister);
		void REP(Registers &r, const Src &integerRegister);
		void WHILE(Registers &r, const Src &temporaryRegister);
		void RET(Registers &r);
		void LEAVE(Registers &r);

		void writeDestination(Registers &r, Vector4i &d, const Dst &dst);
		Vector4i regi(Registers &r, const Src &src);
		Vector4f reg(Registers &r, const Src &src, int offset = 0);
		Vector4f readConstant(Registers &r, const Src &src, int offset = 0);
		Int relativeAddress(Registers &r, const Shader::Parameter &var);
		Int4 enableMask(Registers &r, const Shader::Instruction *instruction);

		bool colorUsed();
		unsigned short shaderVersion() const;
		bool interpolateZ() const;
		bool interpolateW() const;

		const PixelShader *const shader;

	private:
		SamplerCore *sampler[16];

		bool perturbate;
		bool luminance;
		bool previousScaling;

		int ifDepth;
		int loopRepDepth;
		int breakDepth;
		int currentLabel;
		bool whileTest;

		// FIXME: Get rid of llvm::
		llvm::BasicBlock *ifFalseBlock[24 + 24];
		llvm::BasicBlock *loopRepTestBlock[4];
		llvm::BasicBlock *loopRepEndBlock[4];
		llvm::BasicBlock *labelBlock[2048];
		std::vector<llvm::BasicBlock*> callRetBlock[2048];
		llvm::BasicBlock *returnBlock;
		bool isConditionalIf[24 + 24];
	};
}

#endif   // sw_PixelRoutine_hpp
