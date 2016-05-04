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

// Sync.hpp: Defines sync objects for the EGL_KHR_fence_sync extension.

#ifndef LIBEGL_SYNC_H_
#define LIBEGL_SYNC_H_

#include "Context.hpp"

#include <EGL/eglext.h>

namespace egl
{

class FenceSync
{
public:
	explicit FenceSync(Context *context) : context(context)
	{
		status = EGL_UNSIGNALED_KHR;
		context->addRef();
	}

	~FenceSync()
	{
		context->release();
		context = nullptr;
	}

	void wait() { context->finish(); signal(); }
	void signal() { status = EGL_SIGNALED_KHR; }
	bool isSignaled() const { return status == EGL_SIGNALED_KHR; }

private:
	EGLint status;
	Context *context;
};

}

#endif   // LIBEGL_SYNC_H_
