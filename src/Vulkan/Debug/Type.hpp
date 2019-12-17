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

#ifndef VK_DEBUG_TYPE_HPP_
#define VK_DEBUG_TYPE_HPP_

#include <memory>

#include <cstdint>
#include <string>

namespace vk {
namespace dbg {

class VariableContainer;
class Value;

// Kind is an enumerator of type kinds.
enum class Kind
{
	Bool,               // Boolean
	U8,                 // 8-bit unsigned integer.
	S8,                 // 8-bit signed integer.
	U16,                // 16-bit unsigned integer.
	S16,                // 16-bit signed integer.
	F32,                // 32-bit unsigned integer.
	U32,                // 32-bit signed integer.
	S32,                // 32-bit unsigned integer.
	F64,                // 64-bit signed integer.
	U64,                // 64-bit unsigned integer.
	S64,                // 64-bit signed integer.
	Ptr,                // A pointer.
	VariableContainer,  // A VariableContainer.
};

// Type describes the type of a value.
class Type
{
public:
	Type() = default;
	inline Type(Kind kind);
	inline Type(Kind kind, const std::shared_ptr<const Type> &elem);

	// string() returns a string representation of the type.
	std::string string() const;

	const Kind kind;                         // Type kind.
	const std::shared_ptr<const Type> elem;  // Element type of pointer.
};

Type::Type(Kind kind)
    : kind(kind)
{}

Type::Type(Kind kind, const std::shared_ptr<const Type> &elem)
    : kind(kind)
    , elem(elem)
{}

// clang-format off
template <typename T> struct TypeOf;
template <> struct TypeOf<bool>              { static std::shared_ptr<Type> get(); };
template <> struct TypeOf<uint8_t>           { static std::shared_ptr<Type> get(); };
template <> struct TypeOf<int8_t>            { static std::shared_ptr<Type> get(); };
template <> struct TypeOf<uint16_t>          { static std::shared_ptr<Type> get(); };
template <> struct TypeOf<int16_t>           { static std::shared_ptr<Type> get(); };
template <> struct TypeOf<float>             { static std::shared_ptr<Type> get(); };
template <> struct TypeOf<uint32_t>          { static std::shared_ptr<Type> get(); };
template <> struct TypeOf<int32_t>           { static std::shared_ptr<Type> get(); };
template <> struct TypeOf<double>            { static std::shared_ptr<Type> get(); };
template <> struct TypeOf<uint64_t>          { static std::shared_ptr<Type> get(); };
template <> struct TypeOf<int64_t>           { static std::shared_ptr<Type> get(); };
template <> struct TypeOf<VariableContainer> { static std::shared_ptr<Type> get(); };
// clang-format on

}  // namespace dbg
}  // namespace vk

#endif  // VK_DEBUG_TYPE_HPP_