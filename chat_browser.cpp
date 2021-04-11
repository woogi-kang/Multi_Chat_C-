// File Name: echoserver.cpp
// Purpose:   Defines an echo server program.
// Language:  C++
// Platform:  Visual Studio 15.8.2 w/ C++17, g++ (Ubuntu 7.3.0-16ubuntu3) 7.3.0
// Author:    Kang Tae Wook
// Creation date: 2018/11/20

#ifdef _WIN32
#include "sockets.h"
#include <iostream>
#include <array>
#include <thread>
#include <mutex>
#else
#include "sockets.h"
#include <iostream>
#include <array>
#include <thread>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#endif

void receive_Message(sockets::sock socket);

int main(int argc, char const* argv[])
{
	if (argc != 3)
	{
		std::cerr << "usage: " << argv[0] << " <server ip address> <port or service name>\n";
		return 1;
	}

	const char* const server_host = argv[1];
	const char* const port = argv[2];

	const sockets::sock browser_socket = sockets::open_browser_socket(server_host, port);

	if (browser_socket == sockets::BAD_SOCKET)
		return 1;

	std::mutex m1;

	std::array<char, 20> name_of_socket{ "browser" };
	m1.lock();
	send(browser_socket, name_of_socket.data(), name_of_socket.size(), 0);
	m1.unlock();

	std::thread t(receive_Message, browser_socket);
	t.join();

	return 0;
}

void receive_Message(sockets::sock socket)
{
	while (true)
	{
		std::mutex m2;
		std::array<char, 8069> receive_buffer{};
		int bytes_received = 0;

		m2.lock();
		bytes_received = recv(socket, receive_buffer.data(), 1024, 0);

		if (bytes_received > 0)
		{
			std::cout << receive_buffer.data();
			receive_buffer[bytes_received] = 0;
			m2.unlock();
		}

		if (bytes_received <= 0)
		{
			if (bytes_received < 0)
			{
				std::cerr << "Server has shutdown" << std::endl;
				receive_buffer[bytes_received] = 0;
				m2.unlock();
			}
			break;
		}
		receive_buffer[bytes_received] = 0;
	}
	sockets::close_socket(socket);
}