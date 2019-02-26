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
#include "Device/Stream.hpp"
#include "System/Types.hpp"

namespace sw
{
	struct Stream;

	class VertexProgram : public VertexRoutine, public ShaderCore
	{
	public:
		VertexProgram(
			const VertexProcessor::State &state,
			vk::PipelineLayout const *pipelineLayout,
			SpirvShader const *spirvShader);

		virtual ~VertexProgram();

	private:
		Int enableIndex;
		Array<Int4, 1 + 24> enableStack;

		void program(UInt &index) override;
		RValue<Pointer<Byte>> uniformAddress(int bufferIndex, unsigned int index);
		RValue<Pointer<Byte>> uniformAddress(int bufferIndex, unsigned int index, Int &offset);
		Int4 enableMask();

		int ifDepth;
		int loopRepDepth;
		int currentLabel;
		bool whileTest;

		BasicBlock *ifFalseBlock[24 + 24];
		BasicBlock *loopRepTestBlock[4];
		BasicBlock *loopRepEndBlock[4];
		BasicBlock *labelBlock[2048];
		std::vector<BasicBlock*> callRetBlock[2048];
		BasicBlock *returnBlock;
		bool isConditionalIf[24 + 24];
	};
}

#endif   // sw_VertexProgram_hpp
