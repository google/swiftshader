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

#include <spirv/unified1/spirv.hpp>

#define CONCAT(a, b) a##b
#define CONCAT2(a, b) CONCAT(a, b)

namespace {

// checkForNoMissingOps() is an unused function that simply exists to try and
// detect missing opcodes in "SpirvShaderInstructions.inl".
// If there are missing opcodes, then some compilers will warn that not all
// enum values are handled by the switch case below.
constexpr void checkForNoMissingOps(spv::Op op)
{
	// self-reference to avoid unused-function warnings.
	(void)&checkForNoMissingOps;

	switch(op)
	{
#define DECORATE_OP(isStatement, op) \
	case spv::op:                    \
		return;
#include "SpirvShaderInstructions.inl"
#undef DECORATE_OP
		case spv::OpMax: return;
	}
}

}  // anonymous namespace

namespace sw {

bool SpirvShader::IsStatement(spv::Op op)
{
	switch(op)
	{
#define IS_STATEMENT_T(op) case spv::op:
#define IS_STATEMENT_F(op)
#define DECORATE_OP(isStatement, op)    \
	CONCAT2(IS_STATEMENT_, isStatement) \
	(op)
#include "SpirvShaderInstructions.inl"
#undef IS_STATEMENT_T
#undef IS_STATEMENT_F
#undef DECORATE_OP
		return true;

		default:
			return false;
	}
}

}  // namespace sw