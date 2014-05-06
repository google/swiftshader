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

#ifndef D3D9_Direct3DBaseTexture9_hpp
#define D3D9_Direct3DBaseTexture9_hpp

#include "Direct3DResource9.hpp"

#include <d3d9.h>

namespace sw
{
	class Resource;
}

namespace D3D9
{
	class Direct3DBaseTexture9 : public IDirect3DBaseTexture9, public Direct3DResource9
	{
	public:
		Direct3DBaseTexture9(Direct3DDevice9 *device, D3DRESOURCETYPE type, D3DFORMAT format, D3DPOOL pool, unsigned long levels, unsigned long usage);

		virtual ~Direct3DBaseTexture9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DResource9 methods
		long __stdcall GetDevice(IDirect3DDevice9 **device);
		long __stdcall SetPrivateData(const GUID &guid, const void *data, unsigned long size, unsigned long flags);
		long __stdcall GetPrivateData(const GUID &guid, void *data, unsigned long *size);
		long __stdcall FreePrivateData(const GUID &guid);
		unsigned long __stdcall SetPriority(unsigned long newPriority);
		unsigned long __stdcall GetPriority();
		void __stdcall PreLoad();
		D3DRESOURCETYPE __stdcall GetType();

		// IDirect3DBaseTexture9 methods
		unsigned long __stdcall SetLOD(unsigned long newLOD);
		unsigned long __stdcall GetLOD();
		unsigned long __stdcall GetLevelCount();
		long __stdcall SetAutoGenFilterType(D3DTEXTUREFILTERTYPE filterType);
		D3DTEXTUREFILTERTYPE __stdcall GetAutoGenFilterType();
		void __stdcall GenerateMipSubLevels();

		// Intenal methods
		sw::Resource *getResource() const;
		unsigned long getInternalLevelCount() const;
		unsigned long getUsage() const;
		D3DFORMAT getFormat() const;

	protected:
		// Creation parameters
		unsigned long levels;   // Recalculated when 0
		const unsigned long usage;
		const D3DFORMAT format;

		sw::Resource *resource;

	private:
		D3DTEXTUREFILTERTYPE filterType;
		unsigned long LOD;
	};
}

#endif // D3D9_Direct3DBaseTexture9_hpp
