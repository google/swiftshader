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

#ifndef D3D9_Direct3DVolumeTexture9_hpp
#define D3D9_Direct3DVolumeTexture9_hpp

#include "Direct3DBaseTexture9.hpp"

#include "Config.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DVolume9;

	class Direct3DVolumeTexture9 : public IDirect3DVolumeTexture9, public Direct3DBaseTexture9
	{
	public:
		Direct3DVolumeTexture9(Direct3DDevice9 *device, unsigned int width, unsigned int height, unsigned int depth, unsigned int levels, unsigned long usage, D3DFORMAT format, D3DPOOL pool);

		virtual ~Direct3DVolumeTexture9();

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

		// IDirect3DVolumeTexture9 methods
		long __stdcall GetLevelDesc(unsigned int level, D3DVOLUME_DESC *description);
		long __stdcall GetVolumeLevel(unsigned int level, IDirect3DVolume9 **volume);
		long __stdcall LockBox(unsigned int level, D3DLOCKED_BOX *lockedVolume, const D3DBOX *box, unsigned long flags);
		long __stdcall UnlockBox(unsigned int level);
		long __stdcall AddDirtyBox(const D3DBOX *dirtyBox);

		// Internal methods
		Direct3DVolume9 *getInternalVolumeLevel(unsigned int level);

	private:
		// Creation parameters
		const unsigned int width;
		const unsigned int height;
		const unsigned int depth;

		Direct3DVolume9 *volumeLevel[sw::MIPMAP_LEVELS];
	};
}

#endif // D3D9_Direct3DVolumeTexture9_hpp
