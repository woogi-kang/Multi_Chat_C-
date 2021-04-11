// File Name: sockets.cpp
// Purpose:   To implement the platform specifics needed for berkeley sockets.
// Language:  C++
// Platform:  Visual Studio 15.8.2 w/ C++17, g++ (Ubuntu 7.3.0-16ubuntu3) 7.3.0
// Author:    Kang Tae wook
// Creation date: 2018/11/15

#include "sockets.h"
#include <iostream>
//#pragma comment (lib, "Ws2_32.lib")

namespace sockets
{
    sock open_listen_socket(const char* service, Protocol protocol)
	{
        sock listen_socket{BAD_SOCKET};

		addrinfo hints{};
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
		if (protocol == Protocol::IPv4)
			hints.ai_family = AF_INET;
		else
			hints.ai_family = AF_INET6;

		addrinfo* list_results = nullptr;

		auto error = getaddrinfo(NULL, service, &hints, &list_results);
		
		if (error != 0)
		{
			std::cerr << "getaddrinfo error" << gai_strerror(error) << '\n';
			return 1;
		}

		for (auto current_address = list_results; current_address != nullptr;
			current_address = current_address->ai_next)
		{
			listen_socket = socket(current_address->ai_family, current_address->ai_socktype,
				current_address->ai_protocol);

			if (bind(listen_socket, current_address->ai_addr, static_cast<int>(current_address->ai_addrlen)) == 0)
                break; // success
		}

		freeaddrinfo(list_results);
		if (listen_socket != BAD_SOCKET)
		{
			const int LISTEN_BACKLOG = 1024;
			if (listen(listen_socket, LISTEN_BACKLOG) < 0)
			{
				close_socket(listen_socket);
				return BAD_SOCKET;
			}
		}
        return listen_socket;
    }

    sock open_writer_socket(const char* host, const char* service)
    {
        sock client_socket{BAD_SOCKET};
        
		struct addrinfo hints{};
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags |= AI_ADDRCONFIG;

		struct addrinfo* list_results = nullptr;

		auto error = getaddrinfo(host, service, &hints, &list_results);

		if (error != 0)
		{
			std::cerr << "getaddrinfo error" << gai_strerror(error) << '\n';
			return 1;
		}

		for (auto current_address = list_results; current_address != nullptr;
			current_address = current_address->ai_next)
		{
			client_socket = socket(current_address->ai_family, current_address->ai_socktype,
				current_address->ai_protocol);
			if(client_socket == BAD_SOCKET)
				continue;

			if(connect(client_socket, current_address->ai_addr,current_address->ai_addrlen) != -1)
				break;
		}
		
        return client_socket;
    }

	sock open_browser_socket(const char* host, const char* service)
	{
		sock browser_socket{ BAD_SOCKET };

		struct addrinfo hints {};
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags |= AI_ADDRCONFIG;

		struct addrinfo* list_results = nullptr;

		auto error = getaddrinfo(host, service, &hints, &list_results);

		if (error != 0)
		{
			std::cerr << "getaddrinfo error" << gai_strerror(error) << '\n';
			return 1;
		}

		for (auto current_address = list_results; current_address != nullptr;
			current_address = current_address->ai_next)
		{
			browser_socket = socket(current_address->ai_family, current_address->ai_socktype,
				current_address->ai_protocol);
			if (browser_socket == BAD_SOCKET)
				continue;

			if (connect(browser_socket, current_address->ai_addr, current_address->ai_addrlen) != -1)
				break;
		}
		return browser_socket;
	}

    void close_socket(sock socket_handle)
    {
#ifdef _WIN32
		closesocket(socket_handle);
#else
		close(socket_handle);
#endif
    }
}

namespace
{
// ========================================================================
//  This class is designed to be a global singleton that initializes
//  and shuts down Winsock.
// ========================================================================
#ifdef _WIN32
    class WindowsSocketSystem
    {
    public:
        WindowsSocketSystem()
        {
			WSADATA windowsSocketData;
			if (WSAStartup(MAKEWORD(2, 2), &windowsSocketData) != 0)
				return;
        }
        ~WindowsSocketSystem()
        {
			WSACleanup();
        }
    };
    WindowsSocketSystem g_system;
#endif
}