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

#include "Input.h"

#include <algorithm>
#include <cassert>
#include <cstring>

namespace pp
{

Input::Input() : mCount(0), mString(0)
{
}

Input::Input(int count, const char* const string[], const int length[]) :
	mCount(count),
	mString(string)
{
	assert(mCount >= 0);
	mLength.reserve(mCount);
	for (int i = 0; i < mCount; ++i)
	{
		int len = length ? length[i] : -1;
		mLength.push_back(len < 0 ? strlen(mString[i]) : len);
	}
}

int Input::read(char* buf, int maxSize)
{
	int nRead = 0;
	while ((nRead < maxSize) && (mReadLoc.sIndex < mCount))
	{
		int size = mLength[mReadLoc.sIndex] - mReadLoc.cIndex;
		size = std::min(size, maxSize);
		memcpy(buf + nRead, mString[mReadLoc.sIndex] + mReadLoc.cIndex, size);
		nRead += size;
		mReadLoc.cIndex += size;

		// Advance string if we reached the end of current string.
		if (mReadLoc.cIndex == mLength[mReadLoc.sIndex])
		{
			++mReadLoc.sIndex;
			mReadLoc.cIndex = 0;
		}
	}
	return nRead;
}

}  // namespace pp

