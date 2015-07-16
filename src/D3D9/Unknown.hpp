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

#ifndef D3D9_Unknown_hpp
#define D3D9_Unknown_hpp

#include <unknwn.h>

namespace D3D9
{
	class Unknown : IUnknown
	{
	public:
		Unknown();

		virtual ~Unknown();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// Internal methods
		virtual void bind();
		virtual void unbind();

	private:
		volatile long referenceCount;
		volatile long bindCount;
	};
}

#endif   // D3D9_Unknown_hpp
