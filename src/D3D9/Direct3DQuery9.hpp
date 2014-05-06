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

#ifndef D3D9_Direct3DQuery9_hpp
#define D3D9_Direct3DQuery9_hpp

#include "Unknown.hpp"

#include "Renderer.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DDevice9;

	class Direct3DQuery9 : public IDirect3DQuery9, public Unknown
	{
	public:
		Direct3DQuery9(Direct3DDevice9 *device, D3DQUERYTYPE type);

		virtual ~Direct3DQuery9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DQuery9 methods
		long __stdcall GetDevice(IDirect3DDevice9 **device);
		D3DQUERYTYPE __stdcall GetType();
		unsigned long __stdcall GetDataSize();
		long __stdcall Issue(unsigned long flags);
		long __stdcall GetData(void *data, unsigned long size, unsigned long flags);

	private:
		// Creation parameters
		Direct3DDevice9 *const device;
		const D3DQUERYTYPE type;

		sw::Query *query;
	};
}

#endif   // D3D9_Direct3DQuery9_hpp
