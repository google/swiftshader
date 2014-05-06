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

#ifndef D3D9_Direct3DVertexDeclaration9_hpp
#define D3D9_Direct3DVertexDeclaration9_hpp

#include "Unknown.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DDevice9;

	class Direct3DVertexDeclaration9 : public IDirect3DVertexDeclaration9, public Unknown
	{
	public:
		Direct3DVertexDeclaration9(Direct3DDevice9 *device, const D3DVERTEXELEMENT9 *vertexElements);
		Direct3DVertexDeclaration9(Direct3DDevice9 *device, unsigned long FVF);

		virtual ~Direct3DVertexDeclaration9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DVertexDeclaration9 methods
		long __stdcall GetDevice(IDirect3DDevice9 **device);
		long __stdcall GetDeclaration(D3DVERTEXELEMENT9 *declaration, unsigned int *numElements);

		// Internal methods
		unsigned long getFVF() const;
		bool isPreTransformed() const;

	private:
		unsigned long computeFVF();   // If equivalent to FVF, else 0

		// Creation parameters
		Direct3DDevice9 *const device;
		D3DVERTEXELEMENT9 *vertexElement;
		
		int numElements;
		unsigned long FVF;
		bool preTransformed;
	};
}

#endif   // D3D9_Direct3DVertexDeclaration9_hpp
