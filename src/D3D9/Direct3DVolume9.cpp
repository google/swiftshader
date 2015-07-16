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

#include "Direct3DVolume9.hpp"

#include "Direct3DDevice9.hpp"
#include "Direct3DResource9.hpp"
#include "Direct3DVolumeTexture9.hpp"
#include "Direct3DSurface9.hpp"
#include "Resource.hpp"
#include "Debug.hpp"

#include <assert.h>

namespace D3D9
{
	bool isLockable(D3DPOOL pool, unsigned long usage)
	{
		return (pool != D3DPOOL_DEFAULT) || (usage & D3DUSAGE_DYNAMIC);
	}

	Direct3DVolume9::Direct3DVolume9(Direct3DDevice9 *device, Direct3DVolumeTexture9 *container, int width, int height, int depth, D3DFORMAT format, D3DPOOL pool, unsigned long usage) : device(device), Surface(container->getResource(), width, height, depth, translateFormat(format), isLockable(pool, usage), false), container(container), width(width), height(height), depth(depth), format(format), pool(pool), lockable(isLockable(pool, usage)), usage(usage)
	{
		resource = new Direct3DResource9(device, D3DRTYPE_VOLUME, pool, memoryUsage(width, height, depth, format));
		resource->bind();
	}

	Direct3DVolume9::~Direct3DVolume9()
	{
		resource->unbind();
	}

	long __stdcall Direct3DVolume9::QueryInterface(const IID &iid, void **object)
	{
		CriticalSection cs(device);

		TRACE("");

		if(iid == IID_IDirect3DVolume9 ||
		   iid == IID_IUnknown)
		{
			AddRef();
			*object = this;

			return S_OK;
		}

		*object = 0;

		return NOINTERFACE(iid);
	}

	unsigned long __stdcall Direct3DVolume9::AddRef()
	{
		TRACE("");

		return container->AddRef();
	}

	unsigned long __stdcall Direct3DVolume9::Release()
	{
		TRACE("");

		return container->Release();
	}

	long Direct3DVolume9::FreePrivateData(const GUID &guid)
	{
		CriticalSection cs(device);

		TRACE("");

		return resource->FreePrivateData(guid);
	}

	long Direct3DVolume9::GetContainer(const IID &iid, void **container)
	{
		CriticalSection cs(device);

		TRACE("");

		if(!container)
		{
			return INVALIDCALL();
		}

		long result = this->container->QueryInterface(iid, container);

		if(result == S_OK)
		{
			return D3D_OK;
		}

		return INVALIDCALL();
	}

	long Direct3DVolume9::GetDesc(D3DVOLUME_DESC *description)
	{
		CriticalSection cs(device);

		TRACE("");

		if(!description)
		{
			return INVALIDCALL();
		}

		description->Format = format;
		description->Type = D3DRTYPE_VOLUME;
		description->Usage = usage;
		description->Pool = pool;
		description->Width = width;
		description->Height = height;
		description->Depth = depth;

		return D3D_OK;
	}

	long Direct3DVolume9::GetDevice(IDirect3DDevice9 **device)
	{
		CriticalSection cs(this->device);

		TRACE("");

		return resource->GetDevice(device);
	}

	long Direct3DVolume9::GetPrivateData(const GUID &guid, void *data, unsigned long *size)
	{
		CriticalSection cs(device);

		TRACE("");

		return resource->GetPrivateData(guid, data, size);
	}

	long Direct3DVolume9::LockBox(D3DLOCKED_BOX *lockedVolume, const D3DBOX *box, unsigned long flags)
	{
		CriticalSection cs(device);

		TRACE("");

		if(!lockedVolume)
		{
			return INVALIDCALL();
		}

		lockedVolume->RowPitch = 0;
		lockedVolume->SlicePitch = 0;
		lockedVolume->pBits = 0;

		if(!lockable)
		{
			return INVALIDCALL();
		}

		lockedVolume->RowPitch = getExternalPitchB();
		lockedVolume->SlicePitch = getExternalSliceB();

		sw::Lock lock = sw::LOCK_READWRITE;

		if(flags & D3DLOCK_DISCARD)
		{
			lock = sw::LOCK_DISCARD;
		}

		if(flags & D3DLOCK_READONLY)
		{
			lock = sw::LOCK_READONLY;
		}

		if(box)
		{
			lockedVolume->pBits = lockExternal(box->Left, box->Top, box->Front, lock, sw::PUBLIC);
		}
		else
		{
			lockedVolume->pBits = lockExternal(0, 0, 0, lock, sw::PUBLIC);
		}

		return D3D_OK;
	}

	long Direct3DVolume9::SetPrivateData(const GUID &guid, const void *data, unsigned long size, unsigned long flags)
	{
		CriticalSection cs(device);

		TRACE("");

		return resource->SetPrivateData(guid, data, size, flags);
	}

	long Direct3DVolume9::UnlockBox()
	{
		CriticalSection cs(device);

		TRACE("");

		unlockExternal();

		return D3D_OK;
	}

	sw::Format Direct3DVolume9::translateFormat(D3DFORMAT format)
	{
		return Direct3DSurface9::translateFormat(format);
	}

	unsigned int Direct3DVolume9::memoryUsage(int width, int height, int depth, D3DFORMAT format)
	{
		return Surface::size(width, height, depth, translateFormat(format));
	}
}
