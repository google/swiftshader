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

#ifndef sw_PixelProgram_hpp
#define sw_PixelProgram_hpp

#include "PixelRoutine.hpp"

namespace sw
{
	class PixelProgram : public PixelRoutine
	{
	public:
		PixelProgram(const PixelProcessor::State &state, const PixelShader *shader) :
			PixelRoutine(state, shader), ifDepth(0), loopRepDepth(0), breakDepth(0), currentLabel(-1), whileTest(false)
		{
			for(int i = 0; i < 2048; ++i)
			{
				labelBlock[i] = 0;
			}
		}
		virtual ~PixelProgram() {}
	protected:
		virtual void setBuiltins(PixelRoutine::Registers &r, Int &x, Int &y, Float4(&z)[4], Float4 &w);
		virtual void applyShader(PixelRoutine::Registers &r, Int cMask[4]);
		virtual Bool alphaTest(PixelRoutine::Registers &r, Int cMask[4]);
		virtual void rasterOperation(PixelRoutine::Registers &r, Float4 &fog, Pointer<Byte> cBuffer[4], Int &x, Int sMask[4], Int zMask[4], Int cMask[4]);
		virtual QuadRasterizer::Registers* createRegisters(const PixelShader *shader) { return new PixelProgram::Registers(shader); };

	private:
		struct Registers : public PixelRoutine::Registers
		{
			Registers(const PixelShader *shader) :
				PixelRoutine::Registers(shader),
				r(shader && shader->dynamicallyIndexedTemporaries),
				loopDepth(-1)
			{
				enableStack[0] = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);

				if(shader && shader->containsBreakInstruction())
				{
					enableBreak = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
				}

				if(shader && shader->containsContinueInstruction())
				{
					enableContinue = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
				}
			}

			// Temporary registers
			RegisterArray<4096> r;

			// Color outputs
			Vector4f c[RENDERTARGETS];
			RegisterArray<RENDERTARGETS, true> oC;

			// Shader variables
			Vector4f vPos;
			Vector4f vFace;

			// DX9 specific variables
			Vector4f p0;
			Array<Int, 4> aL;
			Array<Int, 4> increment;
			Array<Int, 4> iteration;

			Int loopDepth;    // FIXME: Add support for switch
			Int stackIndex;   // FIXME: Inc/decrement callStack
			Array<UInt, 16> callStack;

			// Per pixel based on conditions reached
			Int enableIndex;
			Array<Int4, 1 + 24> enableStack;
			Int4 enableBreak;
			Int4 enableContinue;
			Int4 enableLeave;
		};

		void sampleTexture(Registers &r, Vector4f &c, const Src &sampler, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, bool project = false, bool bias = false, bool gradients = false, bool lodProvided = false);
		void sampleTexture(Registers &r, Vector4f &c, int sampler, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, bool project = false, bool bias = false, bool gradients = false, bool lodProvided = false);

		// Raster operations
		void clampColor(Vector4f oC[RENDERTARGETS]);

		Int4 enableMask(Registers &r, const Shader::Instruction *instruction);

		Vector4f fetchRegisterF(Registers &r, const Src &src, unsigned int offset = 0);
		Vector4f readConstant(Registers &r, const Src &src, unsigned int offset = 0);
		Int relativeAddress(Registers &r, const Shader::Parameter &var);

		Float4 linearToSRGB(const Float4 &x);

		// Instructions
		typedef Shader::Control Control;

		void M3X2(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1);
		void M3X3(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1);
		void M3X4(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1);
		void M4X3(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1);
		void M4X4(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1);
		void TEXLD(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1, bool project, bool bias);
		void TEXLDD(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1, Vector4f &src2, Vector4f &src3, bool project, bool bias);
		void TEXLDL(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1, bool project, bool bias);
		void TEXSIZE(Registers &r, Vector4f &dst, Float4 &lod, const Src &src1);
		void TEXKILL(Int cMask[4], Vector4f &src, unsigned char mask);
		void TEXOFFSET(Registers &r, Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2, Vector4f &src3, bool project, bool bias);
		void TEXLDL(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1, Vector4f &src2, bool project, bool bias);
		void TEXELFETCH(Registers &r, Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2);
		void TEXELFETCH(Registers &r, Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2, Vector4f &src3);
		void TEXGRAD(Registers &r, Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2, Vector4f &src3);
		void TEXGRAD(Registers &r, Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2, Vector4f &src3, Vector4f &src4);
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

#endif
