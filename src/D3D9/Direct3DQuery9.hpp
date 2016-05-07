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
