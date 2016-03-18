// SwiftShader Software Renderer
//
// Copyright(c) 2005-2012 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef sw_Constants_hpp
#define sw_Constants_hpp

#include "Common/Types.hpp"

namespace sw
{
	struct Constants
	{
		Constants();
		
		unsigned int transposeBit0[16];
		unsigned int transposeBit1[16];
		unsigned int transposeBit2[16];

		ushort4 cWeight[17];
		float4 uvWeight[17];
		float4 uvStart[17];

		unsigned int occlusionCount[16];

		byte8 maskB4Q[16];
		byte8 invMaskB4Q[16];
		word4 maskW4Q[16];
		word4 invMaskW4Q[16];
		dword4 maskD4X[16];
		dword4 invMaskD4X[16];
		qword maskQ0Q[16];
		qword maskQ1Q[16];
		qword maskQ2Q[16];
		qword maskQ3Q[16];
		qword invMaskQ0Q[16];
		qword invMaskQ1Q[16];
		qword invMaskQ2Q[16];
		qword invMaskQ3Q[16];
		dword4 maskX0X[16];
		dword4 maskX1X[16];
		dword4 maskX2X[16];
		dword4 maskX3X[16];
		dword4 invMaskX0X[16];
		dword4 invMaskX1X[16];
		dword4 invMaskX2X[16];
		dword4 invMaskX3X[16];
		dword2 maskD01Q[16];
		dword2 maskD23Q[16];
		dword2 invMaskD01Q[16];
		dword2 invMaskD23Q[16];
		qword2 maskQ01X[16];
		qword2 maskQ23X[16];
		qword2 invMaskQ01X[16];
		qword2 invMaskQ23X[16];
		word4 maskW01Q[4];
		word4 invMaskW01Q[4];
		dword4 maskD01X[4];
		dword4 invMaskD01X[4];
		word4 mask565Q[8];
		word4 invMask565Q[8];

		unsigned short sRGBtoLinear8_12[256];
		unsigned short sRGBtoLinear6_12[64];
		unsigned short sRGBtoLinear5_12[32];

		unsigned short linearToSRGB12_16[4096];
		unsigned short sRGBtoLinear12_16[4096];

		// Centroid parameters
		float4 sampleX[4][16];
		float4 sampleY[4][16];
		float4 weight[16];

		// Fragment offsets
		int Xf[4];
		int Yf[4];

		float4 X[4];
		float4 Y[4];

		dword maxX[16];
		dword maxY[16];
		dword maxZ[16];
		dword minX[16];
		dword minY[16];
		dword minZ[16];
		dword fini[16];

		dword4 maxPos;

		float4 unscaleByte;
		float4 unscaleSByte;
		float4 unscaleShort;
		float4 unscaleUShort;
		float4 unscaleInt;
		float4 unscaleUInt;
		float4 unscaleFixed;

		float half2float[65536];
	};

	extern Constants constants;
}

#endif   // sw_Constants_hpp
