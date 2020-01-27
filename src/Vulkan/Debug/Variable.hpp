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

#ifndef VK_DEBUG_VARIABLE_HPP_
#define VK_DEBUG_VARIABLE_HPP_

#include "ID.hpp"
#include "Type.hpp"
#include "Value.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace vk {
namespace dbg {

// Variable is a named value.
struct Variable
{
	std::string name;
	std::shared_ptr<Value> value;
};

// VariableContainer is a collection of named values.
class VariableContainer : public Value
{
public:
	using ID = dbg::ID<VariableContainer>;

	inline VariableContainer(ID id);

	// foreach() calls cb with each of the variables in the container.
	// F must be a function with the signature void(const Variable&).
	template<typename F>
	inline void foreach(size_t startIndex, size_t count, const F &cb) const;

	// find() looks up the variable with the given name.
	// If the variable with the given name is found, cb is called with the
	// variable and find() returns true.
	template<typename F>
	inline bool find(const std::string &name, const F &cb) const;

	// put() places the variable var into the container.
	inline void put(const Variable &var);

	// put() places the variable with the given name and value into the container.
	inline void put(const std::string &name, const std::shared_ptr<Value> &value);

	// extend() adds base to the list of VariableContainers that will be
	// searched and traversed for variables.
	inline void extend(const std::shared_ptr<VariableContainer> &base);

	// The unique identifier of the variable.
	const ID id;

private:
	struct ForeachIndex
	{
		size_t start;
		size_t count;
	};

	template<typename F>
	inline void foreach(ForeachIndex &index, const F &cb) const;

	inline std::shared_ptr<Type> type() const override;
	inline const void *get() const override;

	mutable std::mutex mutex;
	std::vector<Variable> variables;
	std::unordered_map<std::string, int> indices;
	std::vector<std::shared_ptr<VariableContainer>> extends;
};

VariableContainer::VariableContainer(ID id)
    : id(id)
{}

template<typename F>
void VariableContainer::foreach(size_t startIndex, size_t count, const F &cb) const
{
	auto index = ForeachIndex{ startIndex, count };
	foreach(index, cb);
}

template<typename F>
void VariableContainer::foreach(ForeachIndex &index, const F &cb) const
{
	std::unique_lock<std::mutex> lock(mutex);
	for(size_t i = index.start; i < variables.size() && i < index.count; i++)
	{
		cb(variables[i]);
	}

	index.start -= std::min(index.start, variables.size());
	index.count -= std::min(index.count, variables.size());

	for(auto &base : extends)
	{
		base->foreach(index, cb);
	}
}

template<typename F>
bool VariableContainer::find(const std::string &name, const F &cb) const
{
	std::unique_lock<std::mutex> lock(mutex);
	for(auto const &var : variables)
	{
		if(var.name == name)
		{
			cb(var);
			return true;
		}
	}
	for(auto &base : extends)
	{
		if(base->find(name, cb))
		{
			return true;
		}
	}
	return false;
}

void VariableContainer::put(const Variable &var)
{
	std::unique_lock<std::mutex> lock(mutex);
	auto it = indices.find(var.name);
	if(it == indices.end())
	{
		indices.emplace(var.name, variables.size());
		variables.push_back(var);
	}
	else
	{
		variables[it->second].value = var.value;
	}
}

void VariableContainer::put(const std::string &name,
                            const std::shared_ptr<Value> &value)
{
	put({ name, value });
}

void VariableContainer::extend(const std::shared_ptr<VariableContainer> &base)
{
	std::unique_lock<std::mutex> lock(mutex);
	extends.emplace_back(base);
}

std::shared_ptr<Type> VariableContainer::type() const
{
	return TypeOf<VariableContainer>::get();
}

const void *VariableContainer::get() const
{
	return nullptr;
}

}  // namespace dbg
}  // namespace vk

#endif  // VK_DEBUG_VARIABLE_HPP_