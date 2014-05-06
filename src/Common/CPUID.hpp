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

#ifndef sw_CPUID_hpp
#define sw_CPUID_hpp

namespace sw
{
	#if !defined(__x86_64__) && (defined(_M_AMD64) || defined (_M_X64))
		#define __x86_64__ 1
	#endif

	class CPUID
	{
	public:
		static bool supportsMMX();
		static bool supportsCMOV();
		static bool supportsSSE();
		static bool supportsSSE2();
		static bool supportsSSE3();
		static bool supportsSSSE3();
		static bool supportsSSE4_1();
		static int coreCount();
		static int processAffinity();

		static void setEnableMMX(bool enable);
		static void setEnableCMOV(bool enable);
		static void setEnableSSE(bool enable);
		static void setEnableSSE2(bool enable);
		static void setEnableSSE3(bool enable);
		static void setEnableSSSE3(bool enable);
		static void setEnableSSE4_1(bool enable);

	private:
		static bool MMX;
		static bool CMOV;
		static bool SSE;
		static bool SSE2;
		static bool SSE3;
		static bool SSSE3;
		static bool SSE4_1;
		static int cores;
		static int affinity;

		static bool enableMMX;
		static bool enableCMOV;
		static bool enableSSE;
		static bool enableSSE2;
		static bool enableSSE3;
		static bool enableSSSE3;
		static bool enableSSE4_1;

		static bool detectMMX();
		static bool detectCMOV();
		static bool detectSSE();
		static bool detectSSE2();
		static bool detectSSE3();
		static bool detectSSSE3();
		static bool detectSSE4_1();
		static int detectCoreCount();
		static int detectAffinity();
	};
}

namespace sw
{
	inline bool CPUID::supportsMMX()
	{
		return MMX && enableMMX;
	}

	inline bool CPUID::supportsCMOV()
	{
		return CMOV && enableCMOV;
	}

	inline bool CPUID::supportsSSE()
	{
		return SSE && enableSSE;
	}

	inline bool CPUID::supportsSSE2()
	{
		return SSE2 && enableSSE2;
	}

	inline bool CPUID::supportsSSE3()
	{
		return SSE3 && enableSSE3;
	}

	inline bool CPUID::supportsSSSE3()
	{
		return SSSE3 && enableSSSE3;
	}

	inline bool CPUID::supportsSSE4_1()
	{
		return SSE4_1 && enableSSE4_1;
	}

	inline int CPUID::coreCount()
	{
		return cores;
	}

	inline int CPUID::processAffinity()
	{
		return affinity;
	}
}

#endif   // sw_CPUID_hpp
