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

#ifndef VK_DEBUG_ID_HPP_
#define VK_DEBUG_ID_HPP_

#include <functional>  // std::hash

namespace vk {
namespace dbg {

// ID is a strongly-typed identifier backed by a int.
// The template parameter T is not actually used by the implementation of
// ID; instead it is used to prevent implicit casts between identifiers of
// different T types.
// IDs are typically used as a map key to value of type T.
template<typename T>
class ID
{
public:
	inline ID()
	    : id(0)
	{}
	inline ID(int id)
	    : id(id)
	{}
	inline bool operator==(const ID<T> &rhs) const { return id == rhs.id; }
	inline bool operator!=(const ID<T> &rhs) const { return id != rhs.id; }
	inline bool operator<(const ID<T> &rhs) const { return id < rhs.id; }
	inline ID operator++() { return ID(++id); }
	inline ID operator++(int) { return ID(id++); }

	// value returns the numerical value of the identifier.
	inline int value() const { return id; }

private:
	int id;
};

}  // namespace dbg
}  // namespace vk

namespace std {

// std::hash implementation for vk::dbg::ID<T>
template<typename T>
struct hash<vk::dbg::ID<T> >
{
	std::size_t operator()(const vk::dbg::ID<T> &id) const noexcept
	{
		return std::hash<int>()(id.value());
	}
};

}  // namespace std

#endif  // VK_DEBUG_ID_HPP_
