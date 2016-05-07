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

#ifndef sw_VertexProgram_hpp
#define sw_VertexProgram_hpp

#include "VertexRoutine.hpp"
#include "ShaderCore.hpp"
#include "SamplerCore.hpp"

#include "Stream.hpp"
#include "Types.hpp"

namespace sw
{
	struct Stream;
	class VertexShader;

	class VertexProgram : public VertexRoutine, public ShaderCore
	{
	public:
		VertexProgram(const VertexProcessor::State &state, const VertexShader *vertexShader);

		virtual ~VertexProgram();

	private:
		const VertexShader *const shader;

		RegisterArray<4096> r;   // Temporary registers
		Vector4f a0;
		Array<Int, 4> aL;
		Vector4f p0;

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

		Int instanceID;

		typedef Shader::DestinationParameter Dst;
		typedef Shader::SourceParameter Src;
		typedef Shader::Control Control;
		typedef Shader::Usage Usage;

		void pipeline() override;
		void program();
		void passThrough();

		Vector4f fetchRegister(const Src &src, unsigned int offset = 0);
		Vector4f readConstant(const Src &src, unsigned int offset = 0);
		RValue<Pointer<Byte>> uniformAddress(int bufferIndex, unsigned int index);
		RValue<Pointer<Byte>> uniformAddress(int bufferIndex, unsigned int index, Int& offset);
		Int relativeAddress(const Shader::Parameter &var, int bufferIndex = -1);
		Int4 enableMask(const Shader::Instruction *instruction);

		void M3X2(Vector4f &dst, Vector4f &src0, Src &src1);
		void M3X3(Vector4f &dst, Vector4f &src0, Src &src1);
		void M3X4(Vector4f &dst, Vector4f &src0, Src &src1);
		void M4X3(Vector4f &dst, Vector4f &src0, Src &src1);
		void M4X4(Vector4f &dst, Vector4f &src0, Src &src1);
		void BREAK();
		void BREAKC(Vector4f &src0, Vector4f &src1, Control);
		void BREAKP(const Src &predicateRegister);
		void BREAK(Int4 &condition);
		void CONTINUE();
		void TEST();
		void CALL(int labelIndex, int callSiteIndex);
		void CALLNZ(int labelIndex, int callSiteIndex, const Src &src);
		void CALLNZb(int labelIndex, int callSiteIndex, const Src &boolRegister);
		void CALLNZp(int labelIndex, int callSiteIndex, const Src &predicateRegister);
		void ELSE();
		void ENDIF();
		void ENDLOOP();
		void ENDREP();
		void ENDWHILE();
		void IF(const Src &src);
		void IFb(const Src &boolRegister);
		void IFp(const Src &predicateRegister);
		void IFC(Vector4f &src0, Vector4f &src1, Control);
		void IF(Int4 &condition);
		void LABEL(int labelIndex);
		void LOOP(const Src &integerRegister);
		void REP(const Src &integerRegister);
		void WHILE(const Src &temporaryRegister);
		void RET();
		void LEAVE();
		void TEXLDL(Vector4f &dst, Vector4f &src, const Src&);
		void TEX(Vector4f &dst, Vector4f &src, const Src&);
		void TEXOFFSET(Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2, Vector4f &src3);
		void TEXLDL(Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2);
		void TEXELFETCH(Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2);
		void TEXELFETCH(Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2, Vector4f &src3);
		void TEXGRAD(Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2, Vector4f &src3);
		void TEXGRAD(Vector4f &dst, Vector4f &src, const Src&, Vector4f &src2, Vector4f &src3, Vector4f &src4);
		void TEXSIZE(Vector4f &dst, Float4 &lod, const Src&);

		void sampleTexture(Vector4f &c, const Src &s, Float4 &u, Float4 &v, Float4 &w, Float4 &q, SamplerMethod method);

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
