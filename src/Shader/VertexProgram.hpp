// SwiftShader Software Renderer
//
// Copyright(c) 2005-2012 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef sw_VertexProgram_hpp
#define sw_VertexProgram_hpp

#include "VertexRoutine.hpp"
#include "ShaderCore.hpp"

#include "Stream.hpp"
#include "Types.hpp"

namespace sw
{
	struct Stream;
	class VertexShader;
	class SamplerCore;

	class VertexProgram : public VertexRoutine, public ShaderCore
	{
	public:
		VertexProgram(const VertexProcessor::State &state, const VertexShader *vertexShader);

		virtual ~VertexProgram();

	private:
		typedef Shader::DestinationParameter Dst;
		typedef Shader::SourceParameter Src;
		typedef Shader::Control Control;
		typedef Shader::Usage Usage;

		void pipeline(Registers &r);
		void program(Registers &r);
		void passThrough(Registers &r);

		Vector4f fetchRegisterF(Registers &r, const Src &src, unsigned int offset = 0);
		Vector4f readConstant(Registers &r, const Src &src, unsigned int offset = 0);
		Int relativeAddress(Registers &r, const Shader::Parameter &var);
		Int4 enableMask(Registers &r, const Shader::Instruction *instruction);

		void M3X2(Registers &r, Vector4f &dst, Vector4f &src0, Src &src1);
		void M3X3(Registers &r, Vector4f &dst, Vector4f &src0, Src &src1);
		void M3X4(Registers &r, Vector4f &dst, Vector4f &src0, Src &src1);
		void M4X3(Registers &r, Vector4f &dst, Vector4f &src0, Src &src1);
		void M4X4(Registers &r, Vector4f &dst, Vector4f &src0, Src &src1);
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
		void TEXLDL(Registers &r, Vector4f &dst, Vector4f &src, const Src&);
		void TEX(Registers &r, Vector4f &dst, Vector4f &src, const Src&);
		void TEXOFFSET(Registers &r, Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2, Vector4f &src3);
		void TEXLDL(Registers &r, Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2);
		void TEXELFETCH(Registers &r, Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2);
		void TEXELFETCH(Registers &r, Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2, Vector4f &src3);
		void TEXGRAD(Registers &r, Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2, Vector4f &src3);
		void TEXGRAD(Registers &r, Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2, Vector4f &src3, Vector4f &src4);
		void TEXSIZE(Registers &r, Vector4f &dst, Float4 &lod, const Src&);

		void sampleTexture(Registers &r, Vector4f &c, const Src &s, Float4 &u, Float4 &v, Float4 &w, Float4 &q);

		SamplerCore *sampler[VERTEX_TEXTURE_IMAGE_UNITS];

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

#endif   // sw_VertexProgram_hpp
