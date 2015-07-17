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

#ifndef D3D9_Direct3DPixelShader9_hpp
#define D3D9_Direct3DPixelShader9_hpp

#include "Unknown.hpp"

#include "PixelShader.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DDevice9;

	class Direct3DPixelShader9 : public IDirect3DPixelShader9, public Unknown
	{
	public:
		Direct3DPixelShader9(Direct3DDevice9 *device, const unsigned long *shaderToken);

		virtual ~Direct3DPixelShader9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DPixelShader9 methods
		long __stdcall GetDevice(IDirect3DDevice9 **device);
		long __stdcall GetFunction(void *data, unsigned int *size);

		// Internal methods
		const sw::PixelShader *getPixelShader() const;

	private:
		// Creation parameters
		Direct3DDevice9 *const device;

		unsigned long *shaderToken;
		int tokenCount;

		sw::PixelShader pixelShader;
	};
}

#endif   // D3D9_Direct3DPixelShader9_hpp
