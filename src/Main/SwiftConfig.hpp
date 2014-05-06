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

#ifndef sw_SwiftConfig_hpp
#define sw_SwiftConfig_hpp

#include "Nucleus.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

namespace sw
{
	class SwiftConfig
	{
	public:
		struct Configuration
		{
			int pixelShaderVersion;
			int vertexShaderVersion;
			int textureMemory;
			int identifier;
			int vertexRoutineCacheSize;
			int pixelRoutineCacheSize;
			int setupRoutineCacheSize;
			int vertexCacheSize;
			int textureSampleQuality;
			int mipmapQuality;
			bool perspectiveCorrection;
			int transcendentalPrecision;
			int threadCount;
			bool enableSSE;
			bool enableSSE2;
			bool enableSSE3;
			bool enableSSSE3;
			bool enableSSE4_1;
			Optimization optimization[10];
			bool disableServer;
			bool keepSystemCursor;
			bool forceWindowed;
			bool complementaryDepthBuffer;
			bool postBlendSRGB;
			bool exactColorRounding;
			bool disableAlphaMode;
			bool disable10BitMode;
			int transparencyAntialiasing;
			int frameBufferAPI;
			int shadowMapping;
			bool forceClearRegisters;
		#ifndef NDEBUG
			unsigned int minPrimitives;
			unsigned int maxPrimitives;
		#endif
		};

		SwiftConfig(bool disableServerOverride);

		~SwiftConfig();

		bool hasNewConfiguration(bool reset = true);
		void getConfiguration(Configuration &configuration);

	private:
		enum Status
		{
			OK = 200,
			NotFound = 404
		};

		void createServer();
		void destroyServer();

		static unsigned long __stdcall serverRoutine(void *parameters);

		bool pending(SOCKET socket, int ms);
		void serverLoop();
		void respond(const char *request);
		std::string page();
		std::string profile();
		void send(Status code, std::string body = "");
		void parsePost(const char *post);

		void readConfiguration(bool disableServerOverride = false);
		void writeConfiguration();

		Configuration config;

		HANDLE serverThread;
		volatile bool terminate;
		CRITICAL_SECTION criticalSection;   // Protects reading and writing the configuration settings

		bool newConfig;

		SOCKET listenSocket;
		SOCKET clientSocket;

		int bufferLength;
		char *receiveBuffer;
	};
}

#endif   // sw_SwiftConfig_hpp
