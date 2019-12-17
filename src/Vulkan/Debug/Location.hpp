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

#ifndef VK_DEBUG_LOCATION_HPP_
#define VK_DEBUG_LOCATION_HPP_

#include <memory>

namespace vk {
namespace dbg {

class File;

// Location holds a file path and line number.
struct Location
{
	Location() = default;
	inline Location(int line, const std::shared_ptr<File> &file);

	int line = 0;  // 1 based. 0 represents no line.
	std::shared_ptr<File> file;
};

Location::Location(int line, const std::shared_ptr<File> &file)
    : line(line)
    , file(file)
{}

}  // namespace dbg
}  // namespace vk

#endif  // VK_DEBUG_LOCATION_HPP_