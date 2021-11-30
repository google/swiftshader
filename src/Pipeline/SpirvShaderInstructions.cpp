// Copyright 2020 The SwiftShader Authors. All Rights Reserved.
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

#include "SpirvShader.hpp"

#include "spirv-tools/libspirv.h"

#include <spirv/unified1/spirv.hpp>

namespace sw {

const char *SpirvShader::OpcodeName(spv::Op op)
{
	return spvOpcodeString(op);
}

// This function is used by the shader debugger to determine whether an instruction is steppable.
bool SpirvShader::IsStatement(spv::Op op)
{
	switch(op)
	{
	default:
		// Most statement-like instructions produce a result which has a type.
		// Note OpType* instructions have a result but it is a type itself.
		{
			bool hasResult = false;
			bool hasResultType = false;
			spv::HasResultAndType(op, &hasResult, &hasResultType);

			return hasResult && hasResultType;
		}
		break;

	// Instructions without a result but potential side-effects.
	case spv::OpNop:
	case spv::OpStore:
	case spv::OpCopyMemory:
	case spv::OpCopyMemorySized:
	case spv::OpImageWrite:
	case spv::OpEmitVertex:
	case spv::OpEndPrimitive:
	case spv::OpEmitStreamVertex:
	case spv::OpEndStreamPrimitive:
	case spv::OpControlBarrier:
	case spv::OpMemoryBarrier:
	case spv::OpAtomicStore:
	case spv::OpBranch:
	case spv::OpBranchConditional:
	case spv::OpSwitch:
	case spv::OpKill:
	case spv::OpReturn:
	case spv::OpReturnValue:
	case spv::OpLifetimeStart:
	case spv::OpLifetimeStop:
	case spv::OpGroupWaitEvents:
	case spv::OpCommitReadPipe:
	case spv::OpCommitWritePipe:
	case spv::OpGroupCommitReadPipe:
	case spv::OpGroupCommitWritePipe:
	case spv::OpRetainEvent:
	case spv::OpReleaseEvent:
	case spv::OpSetUserEventStatus:
	case spv::OpCaptureEventProfilingInfo:
	case spv::OpAtomicFlagClear:
	case spv::OpMemoryNamedBarrier:
	case spv::OpTerminateInvocation:
	case spv::OpTraceRayKHR:
	case spv::OpExecuteCallableKHR:
	case spv::OpIgnoreIntersectionKHR:
	case spv::OpTerminateRayKHR:
	case spv::OpTypeRayQueryKHR:
	case spv::OpRayQueryInitializeKHR:
	case spv::OpRayQueryTerminateKHR:
	case spv::OpRayQueryGenerateIntersectionKHR:
	case spv::OpRayQueryConfirmIntersectionKHR:
	case spv::OpWritePackedPrimitiveIndices4x8NV:
	case spv::OpIgnoreIntersectionNV:
	case spv::OpTerminateRayNV:
	case spv::OpTraceNV:
	case spv::OpTraceMotionNV:
	case spv::OpTraceRayMotionNV:
	case spv::OpTypeAccelerationStructureKHR:
	case spv::OpExecuteCallableNV:
	case spv::OpTypeCooperativeMatrixNV:
	case spv::OpCooperativeMatrixStoreNV:
	case spv::OpBeginInvocationInterlockEXT:
	case spv::OpEndInvocationInterlockEXT:
	case spv::OpDemoteToHelperInvocationEXT:
	case spv::OpSamplerImageAddressingModeNV:
	case spv::OpSubgroupBlockWriteINTEL:
	case spv::OpSubgroupImageBlockWriteINTEL:
	case spv::OpSubgroupImageMediaBlockWriteINTEL:
	case spv::OpAssumeTrueKHR:
	case spv::OpTypeVmeImageINTEL:
	case spv::OpTypeAvcImePayloadINTEL:
	case spv::OpTypeAvcRefPayloadINTEL:
	case spv::OpTypeAvcSicPayloadINTEL:
	case spv::OpTypeAvcMcePayloadINTEL:
	case spv::OpTypeAvcMceResultINTEL:
	case spv::OpTypeAvcImeResultINTEL:
	case spv::OpTypeAvcImeResultSingleReferenceStreamoutINTEL:
	case spv::OpTypeAvcImeResultDualReferenceStreamoutINTEL:
	case spv::OpTypeAvcImeSingleReferenceStreaminINTEL:
	case spv::OpTypeAvcImeDualReferenceStreaminINTEL:
	case spv::OpTypeAvcRefResultINTEL:
	case spv::OpTypeAvcSicResultINTEL:
	case spv::OpRestoreMemoryINTEL:
	case spv::OpLoopControlINTEL:
	case spv::OpTypeBufferSurfaceINTEL:
	case spv::OpTypeStructContinuedINTEL:
	case spv::OpConstantCompositeContinuedINTEL:
	case spv::OpSpecConstantCompositeContinuedINTEL:
		return true;
	}
}

}  // namespace sw