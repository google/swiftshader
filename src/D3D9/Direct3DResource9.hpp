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

#ifndef D3D9_Direct3DResource9_hpp
#define D3D9_Direct3DResource9_hpp

#include "Unknown.hpp"

#include <d3d9.h>

#include <map>

namespace D3D9
{
	class Direct3DDevice9;

	class Direct3DResource9 : public IDirect3DResource9, public Unknown
	{
	public:
		Direct3DResource9(Direct3DDevice9 *device, D3DRESOURCETYPE type, D3DPOOL pool, unsigned int size);

		virtual ~Direct3DResource9();

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

		// Internal methods
		static unsigned int getMemoryUsage();
		D3DPOOL getPool() const;

	protected:
		// Creation parameters
		Direct3DDevice9 *const device;
		const D3DRESOURCETYPE type;
		const D3DPOOL pool;
		const unsigned int size;

	private:
		unsigned long priority;

		struct PrivateData
		{
			PrivateData();
			PrivateData(const void *data, int size, bool managed);

			~PrivateData();

			PrivateData &operator=(const PrivateData &privateData);

			void *data;
			unsigned long size;
			bool managed;   // IUnknown interface
		};

		struct CompareGUID
		{
			bool operator()(const GUID& left, const GUID& right) const
			{
				return memcmp(&left, &right, sizeof(GUID)) < 0;
			}
		};

		typedef std::map<GUID, PrivateData, CompareGUID> PrivateDataMap;
		typedef PrivateDataMap::iterator Iterator;
		PrivateDataMap privateData;

		static unsigned int memoryUsage;
	};
}

#endif   // D3D9_Direct3DResource9_hpp
