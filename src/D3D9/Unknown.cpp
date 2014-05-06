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

#include "Debug.hpp"

namespace D3D9
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

	long Unknown::QueryInterface(const IID &iid, void **object)
	{
		if(iid == IID_IUnknown)
		{
			AddRef();
			*object = this;

			return S_OK;
		}
			
		*object = 0;

		return NOINTERFACE(iid);
	}

	unsigned long Unknown::AddRef()
	{
		return InterlockedIncrement(&referenceCount);
	}

	unsigned long Unknown::Release()
	{
		int current = referenceCount;
		
		if(referenceCount > 0)
		{
			current = InterlockedDecrement(&referenceCount);
		}

		if(referenceCount == 0 && bindCount == 0)
		{
			delete this;
		}

		return current;
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