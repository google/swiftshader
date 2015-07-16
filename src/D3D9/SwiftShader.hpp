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

#ifndef D3D9_SwiftShader_hpp
#define D3D9_SwiftShader_hpp

#include "SwiftShader.h"
#include "Unknown.hpp"

namespace D3D9
{
	class Direct3D9;

	class SwiftShader : public ISwiftShaderPrivateV1, public Unknown
	{
	public:
		SwiftShader(Direct3D9 *d3d9);

		virtual ~SwiftShader();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// ISwiftShaderPrivateV1 methods
		long __stdcall RegisterLicenseKey(char *licenseKey);
		long __stdcall SetControlSetting(SWSETTINGTYPE setting, unsigned long value);

	private:
		// Creation parameters
		Direct3D9 *const d3d9;
	};
}

#endif   // D3D9_SwiftShader_hpp
