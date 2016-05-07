// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef D3D9_Direct3DIndexBuffer9_hpp
#define D3D9_Direct3DIndexBuffer9_hpp

#include "Direct3DResource9.hpp"

#include <d3d9.h>

namespace sw
{
	class Resource;
}

namespace D3D9
{
	class Direct3DIndexBuffer9 : public IDirect3DIndexBuffer9, public Direct3DResource9
	{
	public:
		Direct3DIndexBuffer9(Direct3DDevice9 *device, unsigned int length, unsigned long usage, D3DFORMAT format, D3DPOOL pool);

		virtual ~Direct3DIndexBuffer9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DResource9 methods
		long __stdcall FreePrivateData(const GUID &guid);
		long __stdcall GetPrivateData(const GUID &guid, void *data, unsigned long *size);
		void __stdcall PreLoad();
		long __stdcall SetPrivateData(const GUID &guid, const void *data, unsigned long size, unsigned long flags);
		long __stdcall GetDevice(IDirect3DDevice9 **device);
		unsigned long __stdcall SetPriority(unsigned long newPriority);
		unsigned long __stdcall GetPriority();
		D3DRESOURCETYPE __stdcall GetType();

		// IDirect3DIndexBuffer9 methods
		long __stdcall GetDesc(D3DINDEXBUFFER_DESC *description);
		long __stdcall Lock(unsigned int offset, unsigned int size, void **data, unsigned long flags);
		long __stdcall Unlock();
		
		// Internal methods
		sw::Resource *getResource() const;
		bool is32Bit() const;

	private:
		// Creation parameters
		const unsigned int length;
		const long usage;
		const D3DFORMAT format;

		sw::Resource *indexBuffer;
		int lockCount;
	};
}

#endif // D3D9_Direct3DIndexBuffer9_hpp
