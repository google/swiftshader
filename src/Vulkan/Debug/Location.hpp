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
	inline Location(const std::shared_ptr<File> &file, int line, int column = 0);

	inline bool operator==(const Location &o) const;
	inline bool operator!=(const Location &o) const;

	std::shared_ptr<File> file;
	int line = 0;    // 1 based. 0 represents no line.
	int column = 0;  // 1 based. 0 represents no particular column.
};

Location::Location(const std::shared_ptr<File> &file, int line, int column /* = 0 */)
    : file(file)
    , line(line)
    , column(column)
{}

bool Location::operator==(const Location &o) const
{
	return file == o.file && line == o.line && column == o.column;
}

bool Location::operator!=(const Location &o) const
{
	return !(*this == o);
}

}  // namespace dbg
}  // namespace vk

#endif  // VK_DEBUG_LOCATION_HPP_