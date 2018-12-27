// Copyright 2018 The SwiftShader Authors. All Rights Reserved.
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

#include <spirv/unified1/spirv.hpp>
#include "SpirvShader.hpp"
#include "System/Math.hpp"
#include "System/Debug.hpp"
#include "Device/Config.hpp"

namespace sw
{
	volatile int SpirvShader::serialCounter = 1;    // Start at 1, 0 is invalid shader.

	SpirvShader::SpirvShader(InsnStore const &insns) : insns{insns}, serialID{serialCounter++}, modes{}
	{
		// Simplifying assumptions (to be satisfied by earlier transformations)
		// - There is exactly one extrypoint in the module, and it's the one we want
		// - Input / Output interface blocks, builtin or otherwise, have been split.
		// - The only input/output OpVariables present are those used by the entrypoint

		for (auto insn : *this) {
			switch (insn.opcode()) {
				case spv::OpExecutionMode:
					ProcessExecutionMode(insn);
					break;

				case spv::OpTypeVoid:
				case spv::OpTypeBool:
				case spv::OpTypeInt:
				case spv::OpTypeFloat:
				case spv::OpTypeVector:
				case spv::OpTypeMatrix:
				case spv::OpTypeImage:
				case spv::OpTypeSampler:
				case spv::OpTypeSampledImage:
				case spv::OpTypeArray:
				case spv::OpTypeRuntimeArray:
				case spv::OpTypeStruct:
				case spv::OpTypePointer:
				case spv::OpTypeFunction: {
					auto resultId = insn.word(1);
					auto &object = defs[resultId];
					object.kind = Object::Kind::Type;
					object.definition = insn;
					break;
				}

				case spv::OpVariable: {
					auto typeId = insn.word(1);
					auto resultId = insn.word(2);
					auto storageClass = static_cast<spv::StorageClass>(insn.word(3));
					if (insn.wordCount() > 4)
						UNIMPLEMENTED("Variable initializers not yet supported");

					auto &object = defs[resultId];
					object.kind = Object::Kind::Variable;
					object.definition = insn;
					object.storageClass = storageClass;
					break;
				}

				default:
					break;    // This is OK, these passes are intentionally partial
			}
		}
	}

	void SpirvShader::ProcessExecutionMode(InsnIterator insn)
	{
		auto mode = static_cast<spv::ExecutionMode>(insn.word(2));
		switch (mode) {
			case spv::ExecutionModeEarlyFragmentTests:
				modes.EarlyFragmentTests = true;
				break;
			case spv::ExecutionModeDepthReplacing:
				modes.DepthReplacing = true;
				break;
			case spv::ExecutionModeDepthGreater:
				modes.DepthGreater = true;
				break;
			case spv::ExecutionModeDepthLess:
				modes.DepthLess = true;
				break;
			case spv::ExecutionModeDepthUnchanged:
				modes.DepthUnchanged = true;
				break;
			case spv::ExecutionModeLocalSize:
				modes.LocalSizeX = insn.word(3);
				modes.LocalSizeZ = insn.word(5);
				modes.LocalSizeY = insn.word(4);
				break;
			case spv::ExecutionModeOriginUpperLeft:
				// This is always the case for a Vulkan shader. Do nothing.
				break;
			default:
				UNIMPLEMENTED("No other execution modes are permitted");
		}
	}
}