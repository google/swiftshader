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

#ifndef sw_Stream_hpp
#define sw_Stream_hpp

#include "System/Types.hpp"

namespace sw
{
	enum StreamType ENUM_UNDERLYING_TYPE_UNSIGNED_INT
	{
		STREAMTYPE_COLOR,     // 4 normalized unsigned bytes, ZYXW order
		STREAMTYPE_FLOAT,     // Normalization ignored
		STREAMTYPE_BYTE,
		STREAMTYPE_SBYTE,
		STREAMTYPE_SHORT,
		STREAMTYPE_USHORT,
		STREAMTYPE_INT,
		STREAMTYPE_UINT,
		STREAMTYPE_HALF,      // Normalization ignored
		STREAMTYPE_2_10_10_10_INT,
		STREAMTYPE_2_10_10_10_UINT,

		STREAMTYPE_LAST = STREAMTYPE_2_10_10_10_UINT
	};

	struct Stream
	{
		const void *buffer = nullptr;
		unsigned int robustnessSize = 0;
		unsigned int vertexStride = 0;
		unsigned int instanceStride = 0;
		StreamType type = STREAMTYPE_FLOAT;
		unsigned char count = 0;
		bool normalized = false;
		unsigned int offset = 0;
		unsigned int binding = 0;
	};
}

#endif   // sw_Stream_hpp
