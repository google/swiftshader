// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
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

#ifndef COMPILER_PREPROCESSOR_INPUT_H_
#define COMPILER_PREPROCESSOR_INPUT_H_

#include <vector>

namespace pp
{

// Holds and reads input for Lexer.
class Input
{
public:
	Input();
	Input(int count, const char* const string[], const int length[]);

	int count() const { return mCount; }
	const char* string(int index) const { return mString[index]; }
	int length(int index) const { return mLength[index]; }

	int read(char* buf, int maxSize);

	struct Location
	{
		int sIndex;  // String index;
		int cIndex;  // Char index.

		Location() : sIndex(0), cIndex(0) { }
	};
	const Location& readLoc() const { return mReadLoc; }

private:
	// Input.
	int mCount;
	const char* const* mString;
	std::vector<int> mLength;

	Location mReadLoc;
};

}  // namespace pp
#endif  // COMPILER_PREPROCESSOR_INPUT_H_

