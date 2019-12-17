// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
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

// This file contains code used to aid debugging.

#include "SpirvShader.hpp"
#include <spirv/unified1/spirv.h>

// Prototypes for SPIRV-Tools functions that do not have public headers.
// This is a C++ function, so the name is mangled, and signature changes will
// result in a linker error instead of runtime signature mismatches.

// Gets the name of an instruction, without the "Op" prefix.
extern const char *spvOpcodeString(const SpvOp opcode);

namespace sw {

std::string SpirvShader::OpcodeName(spv::Op op)
{
	return spvOpcodeString(static_cast<SpvOp>(op));
}

}  // namespace sw
