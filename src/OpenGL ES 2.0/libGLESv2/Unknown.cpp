// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include "Unknown.hpp"

#include "../common/debug.h"

#include <windows.h>

namespace gl
{
	Unknown::Unknown()
	{
		referenceCount = 0;
		bindCount = 0;
	}

	Unknown::~Unknown()
	{
		ASSERT(referenceCount == 0);
		ASSERT(bindCount == 0);
	}

	void Unknown::addRef()
	{
		InterlockedIncrement(&referenceCount);
	}

	void Unknown::release()
	{
		if(referenceCount > 0)
		{
			InterlockedDecrement(&referenceCount);
		}

		if(referenceCount == 0 && bindCount == 0)
		{
			delete this;
		}
	}

	void Unknown::bind()
	{
		InterlockedIncrement(&bindCount);
	}

	void Unknown::unbind()
	{
		ASSERT(bindCount > 0);

		InterlockedDecrement(&bindCount);

		if(referenceCount == 0 && bindCount == 0)
		{
			delete this;
		}
	}
}
