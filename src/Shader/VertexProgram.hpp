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
#include "Renderer/Stream.hpp"
#include "Common/Types.hpp"

#include <unordered_map>

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

		RegisterArray<NUM_TEMPORARY_REGISTERS> r;   // Temporary registers
		Vector4f a0;
		Array<Int> aL; // loop counter register
		Vector4f p0;

		Array<Int> increment;
		Array<Int> iteration;

		Int loopDepth;
		Int stackIndex;   // FIXME: Inc/decrement callStack
		Array<UInt> callStack;

		Int enableIndex;
		Array<Int4, MAX_SHADER_ENABLE_STACK_SIZE> enableStack;
		Int4 enableBreak;
		Int4 enableContinue;
		Int4 enableLeave;

		Int instanceID;
		Int4 vertexID;

		typedef Shader::DestinationParameter Dst;
		typedef Shader::SourceParameter Src;
		typedef Shader::Control Control;
		typedef Shader::Usage Usage;

		void pipeline(UInt &index) override;
		void program(UInt &index);
		void passThrough();

		Vector4f fetchRegister(const Src &src, unsigned int offset = 0);
		Vector4f readConstant(const Src &src, unsigned int offset = 0);
		RValue<Pointer<Byte>> uniformAddress(int bufferIndex, unsigned int index);
		RValue<Pointer<Byte>> uniformAddress(int bufferIndex, unsigned int index, Int &offset);
		Int relativeAddress(const Shader::Relative &rel, int bufferIndex = -1);
		Int4 dynamicAddress(const Shader::Relative &rel);
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
		void SCALAR();
		void CALL(int labelIndex, int callSiteIndex);
		void CALLNZ(int labelIndex, int callSiteIndex, const Src &src);
		void CALLNZb(int labelIndex, int callSiteIndex, const Src &boolRegister);
		void CALLNZp(int labelIndex, int callSiteIndex, const Src &predicateRegister);
		void ELSE();
		void ENDIF();
		void ENDLOOP();
		void ENDREP();
		void ENDWHILE();
		void ENDSWITCH();
		void IF(const Src &src);
		void IFb(const Src &boolRegister);
		void IFp(const Src &predicateRegister);
		void IFC(Vector4f &src0, Vector4f &src1, Control);
		void IF(Int4 &condition);
		void LABEL(int labelIndex);
		void LOOP(const Src &integerRegister);
		void REP(const Src &integerRegister);
		void WHILE(const Src &temporaryRegister);
		void SWITCH();
		void RET();
		void LEAVE();
		void TEX(Vector4f &dst, Vector4f &src, const Src&);
		void TEXOFFSET(Vector4f &dst, Vector4f &src, const Src&, Vector4f &offset);
		void TEXLOD(Vector4f &dst, Vector4f &src, const Src&, Float4 &lod);
		void TEXLODOFFSET(Vector4f &dst, Vector4f &src, const Src&, Vector4f &offset, Float4 &lod);
		void TEXELFETCH(Vector4f &dst, Vector4f &src, const Src&, Float4 &lod);
		void TEXELFETCHOFFSET(Vector4f &dst, Vector4f &src, const Src&, Vector4f &offset, Float4 &lod);
		void TEXGRAD(Vector4f &dst, Vector4f &src, const Src&, Vector4f &dsx, Vector4f &dsy);
		void TEXGRADOFFSET(Vector4f &dst, Vector4f &src, const Src&, Vector4f &dsx, Vector4f &dsy, Vector4f &offset);
		void TEXSIZE(Vector4f &dst, Float4 &lod, const Src&);

		Vector4f sampleTexture(const Src &s, Vector4f &uvwq, Float4 &lod, Vector4f &dsx, Vector4f &dsy, Vector4f &offset, SamplerFunction function);
		Vector4f sampleTexture(int sampler, Vector4f &uvwq, Float4 &lod, Vector4f &dsx, Vector4f &dsy, Vector4f &offset, SamplerFunction function);

		int ifDepth = 0;
		int loopRepDepth = 0;
		int currentLabel = -1;
		bool scalar = false;

		std::vector<BasicBlock*> ifFalseBlock;
		std::vector<BasicBlock*> loopRepTestBlock;
		std::vector<BasicBlock*> loopRepEndBlock;
		std::vector<BasicBlock*> labelBlock;
		std::unordered_map<unsigned int, std::vector<BasicBlock*>> callRetBlock; // label -> list of call sites
		BasicBlock *returnBlock;
		std::vector<bool> isConditionalIf;
		std::vector<Int4> restoreContinue;
	};
}

#endif   // sw_VertexProgram_hpp
