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

#include "Type.hpp"

namespace vk {
namespace dbg {

// clang-format off
std::shared_ptr<Type> TypeOf<bool>::get()              { static auto ty = std::make_shared<Type>(Kind::Bool); return ty; }
std::shared_ptr<Type> TypeOf<uint8_t>::get()           { static auto ty = std::make_shared<Type>(Kind::U8);   return ty; }
std::shared_ptr<Type> TypeOf<int8_t>::get()            { static auto ty = std::make_shared<Type>(Kind::S8);   return ty; }
std::shared_ptr<Type> TypeOf<uint16_t>::get()          { static auto ty = std::make_shared<Type>(Kind::U16);  return ty; }
std::shared_ptr<Type> TypeOf<int16_t>::get()           { static auto ty = std::make_shared<Type>(Kind::S16);  return ty; }
std::shared_ptr<Type> TypeOf<float>::get()             { static auto ty = std::make_shared<Type>(Kind::F32);  return ty; }
std::shared_ptr<Type> TypeOf<uint32_t>::get()          { static auto ty = std::make_shared<Type>(Kind::U32);  return ty; }
std::shared_ptr<Type> TypeOf<int32_t>::get()           { static auto ty = std::make_shared<Type>(Kind::S32);  return ty; }
std::shared_ptr<Type> TypeOf<double>::get()            { static auto ty = std::make_shared<Type>(Kind::F64);  return ty; }
std::shared_ptr<Type> TypeOf<uint64_t>::get()          { static auto ty = std::make_shared<Type>(Kind::U64);  return ty; }
std::shared_ptr<Type> TypeOf<int64_t>::get()           { static auto ty = std::make_shared<Type>(Kind::S64);  return ty; }
std::shared_ptr<Type> TypeOf<VariableContainer>::get() { static auto ty = std::make_shared<Type>(Kind::VariableContainer); return ty; }
// clang-format on

std::string Type::string() const
{
	switch(kind)
	{
		case Kind::Bool:
			return "bool";
		case Kind::U8:
			return "uint8_t";
		case Kind::S8:
			return "int8_t";
		case Kind::U16:
			return "uint16_t";
		case Kind::S16:
			return "int16_t";
		case Kind::F32:
			return "float";
		case Kind::U32:
			return "uint32_t";
		case Kind::S32:
			return "int32_t";
		case Kind::F64:
			return "double";
		case Kind::U64:
			return "uint64_t";
		case Kind::S64:
			return "int64_t";
		case Kind::Ptr:
			return elem->string() + "*";
		case Kind::VariableContainer:
			return "struct";
	}
	return "";
}

}  // namespace dbg
}  // namespace vk