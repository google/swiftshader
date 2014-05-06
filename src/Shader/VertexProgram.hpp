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
		typedef Shader::Instruction::DestinationParameter Dst;
		typedef Shader::Instruction::SourceParameter Src;
		typedef Shader::Instruction::Operation Op;
		typedef Shader::Instruction::Operation::Control Control;
		typedef Shader::Instruction::Operation::Usage Usage;

		Color4f readConstant(Registers &r, const Src &src, int offset = 0);
		void pipeline(Registers &r);
		void shader(Registers &r);
		void passThrough(Registers &r);

		Color4f reg(Registers &r, const Src &src, int offset = 0);

		void M3X2(Registers &r, Color4f &dst, Color4f &src0, Src &src1);
		void M3X3(Registers &r, Color4f &dst, Color4f &src0, Src &src1);
		void M3X4(Registers &r, Color4f &dst, Color4f &src0, Src &src1);
		void M4X3(Registers &r, Color4f &dst, Color4f &src0, Src &src1);
		void M4X4(Registers &r, Color4f &dst, Color4f &src0, Src &src1);
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
		void TEXLDL(Registers &r, Color4f &dst, Color4f &src, const Src&);

		SamplerCore *sampler[4];

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

		const VertexShader *const vertexShader;
	};
}

#endif   // sw_VertexProgram_hpp
