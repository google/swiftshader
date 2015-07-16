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

#include "SwiftShader.hpp"

#include "Direct3D9.hpp"
#include "Debug.hpp"

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <initguid.h>

DEFINE_GUID(IID_SwiftShaderPrivateV1, 0x761954E6, 0xC357, 0x426d, 0xA6, 0x90, 0x00, 0x50, 0x56, 0xC0, 0x00, 0x08);

extern SWFILTER maximumFilterQuality;
extern SWFILTER maximumMipmapQuality;
extern SWPERSPECTIVE perspectiveQuality;
extern bool disableServer;

namespace D3D9
{
	SwiftShader::SwiftShader(Direct3D9 *d3d9) : d3d9(d3d9)
	{
		InitValidationApp();
	}

	SwiftShader::~SwiftShader()
	{
	}

	long SwiftShader::QueryInterface(const IID &iid, void **object)
	{
		TRACE("");

		if(iid == IID_SwiftShaderPrivateV1 ||
		   iid == IID_IUnknown)
		{
			AddRef();
			*object = this;

			return S_OK;
		}

		*object = 0;

		return NOINTERFACE(iid);
	}

	unsigned long SwiftShader::AddRef()
	{
		TRACE("");

		return Unknown::AddRef();
	}

	unsigned long SwiftShader::Release()
	{
		TRACE("");

		return Unknown::Release();
	}

	long SwiftShader::RegisterLicenseKey(char *licenseKey)
	{
		TRACE("");

		memset(validationKey, '\0', sizeof(validationKey));
		strncpy(validationKey, licenseKey, strlen(licenseKey));

		return D3D_OK;
	}

	long SwiftShader::SetControlSetting(SWSETTINGTYPE setting, unsigned long value)
	{
		switch(setting)
		{
		case SWS_MAXIMUMFILTERQUALITY:
			maximumFilterQuality = (SWFILTER)value;
			break;
		case SWS_MAXIMUMMIPMAPQUALITY:
			maximumMipmapQuality = (SWFILTER)value;
			break;
		case SWS_PERSPECTIVEQUALITY:
			perspectiveQuality = (SWPERSPECTIVE)value;
			break;
		case SWS_DISABLESERVER:
			disableServer = (value != FALSE);
			break;
		default:
			ASSERT(false);
		}

		return D3D_OK;
	}
}
