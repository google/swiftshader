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

#ifndef _SWIFTSHADER_H_
#define _SWIFTSHADER_H_

#include "Register.hpp"

#include <objbase.h>
#include <stdlib.h>

#define SWIFTSHADERAPI WINAPI

/*
 * Interface IID's
 */
#if defined( _WIN32 ) && !defined( _NO_COM)

/* IID_SwiftShaderPrivateV1 */
/* {761954E6-C357-11DA-A690-005056C00008} */
DEFINE_GUID(IID_SwiftShaderPrivateV1, 0x761954E6, 0xC357, 0x426d, 0xA6, 0x90, 0x00, 0x50, 0x56, 0xC0, 0x00, 0x08);

#endif

#ifdef __cplusplus

#ifndef DECLSPEC_UUID
#if _MSC_VER >= 1100
#define DECLSPEC_UUID(x)    __declspec(uuid(x))
#else
#define DECLSPEC_UUID(x)
#endif
#endif

interface DECLSPEC_UUID("761954E6-C357-11DA-A690-005056C00008") IID_SwiftShaderPrivateV1;


#if defined(_COM_SMARTPTR_TYPEDEF)
_COM_SMARTPTR_TYPEDEF(ISwiftShaderPrivateV1, __uuidof(ISwiftShaderPrivateV1));
#endif
#endif


typedef interface ISwiftShaderPrivateV1  ISwiftShaderPrivateV1;


#ifdef __cplusplus
extern "C" {
#endif


/*
 * SwiftShader Private interface
 */
 
typedef enum _SWSETTINGTYPE {
	SWS_MAXIMUMFILTERQUALITY	= 1, /* SWFILTER texture filtering quality */
	SWS_MAXIMUMMIPMAPQUALITY	= 2, /* SWFILTER mipmap filtering quality */
	SWS_PERSPECTIVEQUALITY		= 3, /* SWPERSPECTIVE perspective correction quality */
	SWS_DISABLESERVER			= 4, /* BOOL disable SwiftConfig server */
	SWS_FORCE_DWORD				= 0x7fffffff, /* force 32-bit size enum */
} SWSETTINGTYPE;

typedef enum _SWFILTER {
	SWF_DEFAULT		= 0, /* Default filter quality (build dependent) */
	SWF_NONE		= 1, /* No filtering (texture/mipmap) */
	SWF_POINT		= 2, /* Point filtering (texture/mipmap) */
	SWF_AVERAGE2	= 3, /* Averaging 2 point samples (texture) */
	SWF_AVERAGE4	= 4, /* Averaging 4 point samples (texture) */
	SWF_POLYGON		= 5, /* Per-polygon mipmapping */
	SWF_LINEAR		= 6, /* (Bi)linear filtering (texture/mipamp) */
	SWF_MAXIMUM		= 7, /* No limitation on filter quality */
	SWF_FORCE_DWORD	= 0x7fffffff, /* force 32-bit size enum */
} SWFILTER;

typedef enum _SWPERSPECTIVE {
	SWP_DEFAULT		= 0, /* Default perspective correction quality */
	SWP_NONE		= 1, /* No perspective correction (affine texture mapping) */
	SWP_FAST		= 2, /* Fast low-precision perspective correction */
	SWP_ACCURATE	= 3, /* Slower high-precision perspective correction */
	SWP_FORCE_DWORD	= 0x7fffffff, /* force 32-bit size enum */
} SWPERSPECTIVE;

#undef INTERFACE
#define INTERFACE ISwiftShaderPrivateV1

DECLARE_INTERFACE_(ISwiftShaderPrivateV1, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** ISwiftShaderPrivateV1 methods ***/
    STDMETHOD(RegisterLicenseKey)(THIS_ char* pLicenseKey) PURE;
	STDMETHOD(SetControlSetting)(THIS_ SWSETTINGTYPE Setting,DWORD Value) PURE;
};
    
typedef struct ISwiftShaderPrivateV1 *LPSWIFTSHADERPRIVATEV1, *LPSWIFTSHADERPRIVATEV1;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define ISwiftShaderPrivateV1_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define ISwiftShaderPrivateV1_AddRef(p) (p)->lpVtbl->AddRef(p)
#define ISwiftShaderPrivateV1_Release(p) (p)->lpVtbl->Release(p)
#define ISwiftShaderPrivateV1_RegisterLicenseKey(p,a) (p)->lpVtbl->RegisterLicenseKey(p,a)
#else
#define ISwiftShaderPrivateV1_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define ISwiftShaderPrivateV1_AddRef(p) (p)->AddRef()
#define ISwiftShaderPrivateV1_Release(p) (p)->Release()
#define ISwiftShaderPrivateV1_RegisterLicenseKey(p,a) (p)->RegisterLicenseKey(a)
#endif



/****************************************************************************
 * Flags for SwiftShader 
 ****************************************************************************/
#define SWIFTSHADER_SOME_RANDOM_FLAG 0x00000001L


#ifdef __cplusplus
};
#endif

#endif /* _SWIFTSHADER_H_ */

