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

#ifndef sw_Socket_hpp
#define sw_Socket_hpp

#if defined(_WIN32)
	#include <winsock2.h>
#else
	#include <sys/socket.h>
	typedef int SOCKET;
#endif

namespace sw
{
	class Socket
	{
	public:
		Socket(SOCKET socket);
		Socket(const char *address, const char *port);
		~Socket();

		void listen(int backlog = 1);
		bool select(int us);
		Socket *accept();
		
		int receive(char *buffer, int length);
		void send(const char *buffer, int length);

		static void startup();
		static void cleanup();

	private:
		SOCKET socket;
	};
}

#endif   // sw_Socket_hpp
