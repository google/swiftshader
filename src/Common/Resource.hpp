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

#ifndef sw_Resource_hpp
#define sw_Resource_hpp

#include "MutexLock.hpp"

namespace sw
{
	enum Accessor
	{
		PUBLIC,    // Application/API access
		PRIVATE,   // Renderer access, shared by multiple threads if read-only
		MANAGED,   // Renderer access, shared read/write access if partitioned
		DESTRUCT
	};

	class Resource
	{
	public:
		Resource(size_t bytes);

		void destruct();   // Asynchronous destructor

		void *lock(Accessor claimer);
		void *lock(Accessor relinquisher, Accessor claimer);
		void unlock();
		void unlock(Accessor relinquisher);

		const void *data() const;
		const size_t size;

	private:
		~Resource();   // Always call destruct() instead

		BackoffLock criticalSection;
		Event unblock;
		volatile int blocked;

		volatile Accessor accessor;
		volatile int count;
		bool orphaned;

		void *buffer;
	};
}

#endif   // sw_Resource_hpp
