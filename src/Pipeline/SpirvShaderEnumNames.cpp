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

#include "spirv-tools/libspirv.h"
#include "spirv/unified1/spirv.h"

namespace sw {

std::string SpirvShader::OpcodeName(spv::Op op)
{
	return spvOpcodeString(static_cast<SpvOp>(op));
}

}  // namespace sw
