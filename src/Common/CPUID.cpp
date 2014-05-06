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

#include "CPUID.hpp"

#if defined(_WIN32)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
	#include <intrin.h>
#elif defined(__APPLE__)
	#include <sched.h>
	#include <sys/types.h>
	#include <sys/sysctl.h>
#else
	#error Unimplemented platform
#endif

namespace sw
{
	bool CPUID::MMX = detectMMX();
	bool CPUID::CMOV = detectCMOV();
	bool CPUID::SSE = detectSSE();
	bool CPUID::SSE2 = detectSSE2();
	bool CPUID::SSE3 = detectSSE3();
	bool CPUID::SSSE3 = detectSSSE3();
	bool CPUID::SSE4_1 = detectSSE4_1();
	int CPUID::cores = detectCoreCount();
	int CPUID::affinity = detectAffinity();

	bool CPUID::enableMMX = true;
	bool CPUID::enableCMOV = true;
	bool CPUID::enableSSE = true;
	bool CPUID::enableSSE2 = true;
	bool CPUID::enableSSE3 = true;
	bool CPUID::enableSSSE3 = true;
	bool CPUID::enableSSE4_1 = true;

	void CPUID::setEnableMMX(bool enable)
	{
		enableMMX = enable;

		if(!enableMMX)
		{
			enableSSE = false;
			enableSSE2 = false;
			enableSSE3 = false;
			enableSSSE3 = false;
			enableSSE4_1 = false;
		}
	}

	void CPUID::setEnableCMOV(bool enable)
	{
		enableCMOV = enable;

		if(!CMOV)
		{
			enableSSE = false;
			enableSSE2 = false;
			enableSSE3 = false;
			enableSSSE3 = false;
			enableSSE4_1 = false;
		}
	}

	void CPUID::setEnableSSE(bool enable)
	{
		SSE = enable;

		if(enableSSE)
		{
			enableMMX = true;
			enableCMOV = true;
		}
		else
		{
			enableSSE2 = false;
			enableSSE3 = false;
			enableSSSE3 = false;
			enableSSE4_1 = false;
		}
	}

	void CPUID::setEnableSSE2(bool enable)
	{
		enableSSE2 = enable;

		if(enableSSE2)
		{
			enableMMX = true;
			enableCMOV = true;
			enableSSE = true;
		}
		else
		{
			enableSSE3 = false;
			enableSSSE3 = false;
			enableSSE4_1 = false;
		}
	}

	void CPUID::setEnableSSE3(bool enable)
	{
		SSE3 = enable;

		if(enableSSE3)
		{
			enableMMX = true;
			enableCMOV = true;
			enableSSE = true;
			enableSSE2 = true;
		}
		else
		{
			enableSSSE3 = false;
			enableSSE4_1 = false;
		}
	}

	void CPUID::setEnableSSSE3(bool enable)
	{
		enableSSSE3 = enable;

		if(enableSSSE3)
		{
			enableMMX = true;
			enableCMOV = true;
			enableSSE = true;
			enableSSE2 = true;
			enableSSE3 = true;
		}
		else
		{
			enableSSE4_1 = false;
		}
	}

	void CPUID::setEnableSSE4_1(bool enable)
	{
		enableSSE4_1 = enable;

		if(enableSSE4_1)
		{
			enableMMX = true;
			enableCMOV = true;
			enableSSE = true;
			enableSSE2 = true;
			enableSSE3 = true;
			enableSSSE3 = true;
		}
	}

	bool CPUID::detectMMX()
	{
		#if defined(_WIN32)
			int registers[4];
			__cpuid(registers, 1);
			return MMX = (registers[3] & 0x00800000) != 0;
		#elif defined(__APPLE__)
			int MMX = false;
			size_t length = sizeof(MMX);
			sysctlbyname("hw.optional.mmx", &MMX, &length, 0, 0);
		#else
			#error Unimplemented platform
		#endif

		return MMX;
	}

	bool CPUID::detectCMOV()
	{
		#if defined(_WIN32)
			int registers[4];
			__cpuid(registers, 1);
			return CMOV = (registers[3] & 0x00008000) != 0;
		#elif defined(__APPLE__)
			int CMOV = false;
			size_t length = sizeof(CMOV);
			sysctlbyname("hw.optional.floatingpoint", &CMOV, &length, 0, 0);
		#else
			#error Unimplemented platform
		#endif

		return CMOV;
	}

	bool CPUID::detectSSE()
	{
		#if defined(_WIN32)
			int registers[4];
			__cpuid(registers, 1);
			return SSE = (registers[3] & 0x02000000) != 0;
		#elif defined(__APPLE__)
			int SSE = false;
			size_t length = sizeof(SSE);
			sysctlbyname("hw.optional.sse", &SSE, &length, 0, 0);
		#else
			#error Unimplemented platform
		#endif

		return SSE;
	}

	bool CPUID::detectSSE2()
	{
		#if defined(_WIN32)
			int registers[4];
			__cpuid(registers, 1);
			return SSE2 = (registers[3] & 0x04000000) != 0;
		#elif defined(__APPLE__)
			int SSE2 = false;
			size_t length = sizeof(SSE2);
			sysctlbyname("hw.optional.sse2", &SSE2, &length, 0, 0);
		#else
			#error Unimplemented platform
		#endif

		return SSE2;
	}

	bool CPUID::detectSSE3()
	{
		#if defined(_WIN32)
			int registers[4];
			__cpuid(registers, 1);
			return SSE3 = (registers[2] & 0x00000001) != 0;
		#elif defined(__APPLE__)
			int SSE3 = false;
			size_t length = sizeof(SSE3);
			sysctlbyname("hw.optional.sse3", &SSE3, &length, 0, 0);
		#else
			#error Unimplemented platform
		#endif

		return SSE3;
	}

	bool CPUID::detectSSSE3()
	{
		#if defined(_WIN32)
			int registers[4];
			__cpuid(registers, 1);
			return SSSE3 = (registers[2] & 0x00000200) != 0;
		#elif defined(__APPLE__)
			int SSSE3 = false;
			size_t length = sizeof(SSSE3);
			sysctlbyname("hw.optional.supplementalsse3", &SSSE3, &length, 0, 0);
		#else
			#error Unimplemented platform
		#endif

		return SSSE3;
	}

	bool CPUID::detectSSE4_1()
	{
		#if defined(_WIN32)
			int registers[4];
			__cpuid(registers, 1);
			return SSE4_1 = (registers[2] & 0x00080000) != 0;
		#elif defined(__APPLE__)
			int SSE4_1 = false;
			size_t length = sizeof(SSE4_1);
			sysctlbyname("hw.optional.sse4_1", &SSE4_1, &length, 0, 0);
		#else
			#error Unimplemented platform
		#endif

		return SSE4_1;
	}

	int CPUID::detectCoreCount()
	{
		int cores = 0;

		#if defined(_WIN32)
			DWORD_PTR processAffinityMask = 1;
			DWORD_PTR systemAffinityMask = 1;
			
			GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask);

			while(systemAffinityMask)
			{
				if(systemAffinityMask & 1)
				{
					cores++;
				}

				systemAffinityMask >>= 1;
			}
		#elif defined(__APPLE__)
			int MIB[2];
			MIB[0] = CTL_HW;
			MIB[1] = HW_NCPU;
			
			size_t length = sizeof(cores);
			sysctl(MIB, 2, &cores, &length, 0, 0);
		#else
			#error Unimplemented platform
		#endif

		if(cores < 1)  cores = 1;
		if(cores > 16) cores = 16;

		return cores;   // FIXME: Number of physical cores
	}

	int CPUID::detectAffinity()
	{
		int cores = 0;

		#if defined(_WIN32)
			DWORD_PTR processAffinityMask = 1;
			DWORD_PTR systemAffinityMask = 1;
			
			GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask);

			while(processAffinityMask)
			{
				if(processAffinityMask & 1)
				{
					cores++;
				}

				processAffinityMask >>= 1;
			}
		#elif defined(__APPLE__)
			return detectCoreCount();   // FIXME: Assumes no affinity limitation	
		#else
			#error Unimplemented platform
		#endif

		if(cores < 1)  cores = 1;
		if(cores > 16) cores = 16;

		return cores;
	}
}
