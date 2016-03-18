// SwiftShader Software Renderer
//
// Copyright(c) 2005-2012 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef sw_Stream_hpp
#define sw_Stream_hpp

#include "Common/Types.hpp"

namespace sw
{
	class Resource;

	enum StreamType : unsigned int
	{
		STREAMTYPE_COLOR,     // 4 normalized unsigned bytes, ZYXW order
		STREAMTYPE_UDEC3,     // 3 unsigned 10-bit fields
		STREAMTYPE_DEC3N,     // 3 normalized signed 10-bit fields
		STREAMTYPE_INDICES,   // 4 unsigned bytes, stored unconverted into X component
		STREAMTYPE_FLOAT,     // Normalization ignored
		STREAMTYPE_BYTE,
		STREAMTYPE_SBYTE,
		STREAMTYPE_SHORT,
		STREAMTYPE_USHORT,
		STREAMTYPE_INT,
		STREAMTYPE_UINT,
		STREAMTYPE_FIXED,     // Normalization ignored (16.16 format)
		STREAMTYPE_HALF,      // Normalization ignored

		STREAMTYPE_LAST = STREAMTYPE_HALF
	};

	struct StreamResource
	{
		Resource *resource;
		const void *buffer;
		unsigned int stride;
	};

	struct Stream : public StreamResource
	{
		Stream(Resource *resource = 0, const void *buffer = 0, unsigned int stride = 0)
		{
			this->resource = resource;
			this->buffer = buffer;
			this->stride = stride;
		}

		Stream &define(StreamType type, unsigned int count, bool normalized = false)
		{
			this->type = type;
			this->count = count;
			this->normalized = normalized;

			return *this;
		}

		Stream &define(const void *buffer, StreamType type, unsigned int count, bool normalized = false)
		{
			this->buffer = buffer;
			this->type = type;
			this->count = count;
			this->normalized = normalized;

			return *this;
		}

		Stream &defaults()
		{
			static const float4 null = {0, 0, 0, 1};
	
			resource = 0;
			buffer = &null;
			stride = 0;
			type = STREAMTYPE_FLOAT;
			count = 0;
			normalized = false;

			return *this;
		}

		operator bool() const   // Returns true if stream contains data
		{
			return count != 0;
		}

		StreamType type;
		unsigned char count;
		bool normalized;
	};
}

#endif   // sw_Stream_hpp
