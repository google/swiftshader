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

#include "Value.hpp"
#include "Type.hpp"
#include "Variable.hpp"

namespace vk {
namespace dbg {

const FormatFlags FormatFlags::Default = {
	"[",                    // listPrefix
	"]",                    // listSuffix
	", ",                   // listDelimiter
	"",                     // listIndent
	&FormatFlags::Default,  // subListFmt
};

std::string Value::string(const FormatFlags &fmt /* = FormatFlags::Default */) const
{
	switch(type()->kind)
	{
		case Kind::Bool:
			return *reinterpret_cast<const bool *>(get()) ? "true" : "false";
		case Kind::U8:
			return std::to_string(*reinterpret_cast<const uint8_t *>(get()));
		case Kind::S8:
			return std::to_string(*reinterpret_cast<const int8_t *>(get()));
		case Kind::U16:
			return std::to_string(*reinterpret_cast<const uint16_t *>(get()));
		case Kind::S16:
			return std::to_string(*reinterpret_cast<const int16_t *>(get()));
		case Kind::F32:
			return std::to_string(*reinterpret_cast<const float *>(get()));
		case Kind::U32:
			return std::to_string(*reinterpret_cast<const uint32_t *>(get()));
		case Kind::S32:
			return std::to_string(*reinterpret_cast<const int32_t *>(get()));
		case Kind::F64:
			return std::to_string(*reinterpret_cast<const double *>(get()));
		case Kind::U64:
			return std::to_string(*reinterpret_cast<const uint64_t *>(get()));
		case Kind::S64:
			return std::to_string(*reinterpret_cast<const int64_t *>(get()));
		case Kind::Ptr:
			return std::to_string(reinterpret_cast<uintptr_t>(get()));
		case Kind::VariableContainer:
		{
			auto const *vc = static_cast<const VariableContainer *>(this);
			std::string out = "";
			auto subfmt = *fmt.subListFmt;
			subfmt.listIndent = fmt.listIndent + fmt.subListFmt->listIndent;
			bool first = true;
			vc->foreach(0, ~0, [&](const Variable &var) {
				if(!first) { out += fmt.listDelimiter; }
				first = false;
				out += fmt.listIndent;
				out += var.name;
				out += ": ";
				out += var.value->string(subfmt);
			});
			return fmt.listPrefix + out + fmt.listSuffix;
		}
	}
	return "";
}

}  // namespace dbg
}  // namespace vk