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

#include "Socket.hpp"

#if defined(_WIN32)
	#include <ws2tcpip.h>
#else
	#include <unistd.h>
	#include <netdb.h>
	#include <netinet/in.h>
#endif

namespace sw
{
	Socket::Socket(SOCKET socket) : socket(socket)
	{
	}

	Socket::Socket(const char *address, const char *port)
	{
		#if defined(_WIN32)
			socket = INVALID_SOCKET;
		#else
			socket = -1;
		#endif

		addrinfo hints = {};
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		addrinfo *info = 0;
		getaddrinfo(address, port, &hints, &info);

		if(info)
		{
			socket = ::socket(info->ai_family, info->ai_socktype, info->ai_protocol);
			bind(socket, info->ai_addr, (int)info->ai_addrlen);
		}
	}

	Socket::~Socket()
	{
		#if defined(_WIN32)
			closesocket(socket);
		#else
			close(socket);
		#endif
	}

	void Socket::listen(int backlog)
	{
		::listen(socket, backlog);
	}

	bool Socket::select(int us)
	{
		fd_set sockets;
		FD_ZERO(&sockets);
		FD_SET(socket, &sockets);
		
		timeval timeout = {us / 1000000, us % 1000000};

		return ::select(FD_SETSIZE, &sockets, 0, 0, &timeout) >= 1;
	}

	Socket *Socket::accept()
	{
		return new Socket(::accept(socket, 0, 0));
	}

	int Socket::receive(char *buffer, int length)
	{
		return recv(socket, buffer, length, 0);
	}

	void Socket::send(const char *buffer, int length)
	{
		::send(socket, buffer, length, 0);
	}

	void Socket::startup()
	{
		#if defined(_WIN32)
			WSADATA winsockData;
			WSAStartup(MAKEWORD(2, 2), &winsockData);
		#endif
	}

	void Socket::cleanup()
	{
		#if defined(_WIN32)
			WSACleanup();
		#endif
	}
}
