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

#ifndef VK_DEBUG_VALUE_HPP_
#define VK_DEBUG_VALUE_HPP_

#include "Type.hpp"

#include <memory>
#include <string>

namespace vk {
namespace dbg {

// FormatFlags holds settings used to serialize a Value to a string.
struct FormatFlags
{
	// The default FormatFlags used to serialize a Value to a string.
	static const FormatFlags Default;

	std::string listPrefix;         // Prefix to lists.
	std::string listSuffix;         // Suffix to lists.
	std::string listDelimiter;      // List item delimiter.
	std::string listIndent;         // List item indententation prefix.
	const FormatFlags *subListFmt;  // Format used for list sub items.
};

// Value holds a value that can be read and possible written to.
class Value
{
public:
	virtual ~Value() = default;

	// type() returns the value's type.
	virtual std::shared_ptr<Type> type() const = 0;

	// string() returns a string representation of the value using the specified
	// FormatFlags.
	virtual std::string string(const FormatFlags & = FormatFlags::Default) const;

	// get() returns a pointer to the value.
	virtual const void *get() const = 0;

	// set() changes the value to a copy of the value at ptr.
	// set() returns true if the value was changed, or false if the value cannot
	// be set.
	virtual bool set(void *ptr) { return false; }
};

// Constant is an immutable value.
template<typename T>
class Constant : public Value
{
public:
	inline Constant(const T &value);
	inline std::shared_ptr<Type> type() const override;
	inline const void *get() const override;

private:
	const T value;
};

template<typename T>
Constant<T>::Constant(const T &value)
    : value(value)
{
}

template<typename T>
std::shared_ptr<Type> Constant<T>::type() const
{
	return TypeOf<T>::get();
}

template<typename T>
const void *Constant<T>::get() const
{
	return &value;
}

// Reference is reference to a value in memory.
template<typename T>
class Reference : public Value
{
public:
	inline Reference(T &ptr);
	inline std::shared_ptr<Type> type() const override;
	inline const void *get() const override;
	inline bool set(void *ptr) override;

private:
	T &ref;
};

template<typename T>
Reference<T>::Reference(T &ref)
    : ref(ref)
{
}

template<typename T>
std::shared_ptr<Type> Reference<T>::type() const
{
	return TypeOf<T>::get();
}

template<typename T>
const void *Reference<T>::get() const
{
	return &ref;
}

template<typename T>
bool Reference<T>::set(void *ptr)
{
	ref = *reinterpret_cast<const T *>(ptr);
	return true;
}

// make_constant() returns a shared_ptr to a Constant with the given value.
template<typename T>
inline std::shared_ptr<Constant<T>> make_constant(const T &value)
{
	return std::make_shared<Constant<T>>(value);
}

// make_reference() returns a shared_ptr to a Reference with the given value.
template<typename T>
inline std::shared_ptr<Reference<T>> make_reference(T &value)
{
	return std::make_shared<Reference<T>>(value);
}

}  // namespace dbg
}  // namespace vk

#endif  // VK_DEBUG_VALUE_HPP_