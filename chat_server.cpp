// File Name: echoserver.cpp
// Purpose:   Defines an echo server program.
// Language:  C++
// Platform:  Visual Studio 15.8.2 w/ C++17, g++ (Ubuntu 7.3.0-16ubuntu3) 7.3.0
// Author:    Kang Tae Wook
// Creation date: 2018/11/20

#ifdef _WIN32
#include "sockets.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#else
#include "sockets.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#endif

namespace
{
	void sendToBrowserAndGetFromBrowser(sockets::sock writer);
	void print_client_information(sockaddr_storage client_address, socklen_t socket_address_storage_size);
	void RespondToClientOrBrowser(sockets::sock conneted_socket);
}

std::string logInfo;
std::vector<sockets::sock> browser_socket;
std::vector<sockets::sock> writer_socket;

int main(int argc, char const* argv[])
{
	if (argc != 2)
	{
		std::cerr << "usage: " << argv[0] << " <port or service name>\n";
		return 1;
	}

	std::cout << "Welcome to chat_Server!" << std::endl;

	const char* port = argv[1];
	const sockets::sock listen_socket = open_listen_socket(port, sockets::Protocol::IPv4);
	socklen_t socket_address_storage_size = sizeof(sockaddr_storage);
	bool should_run = true;

	while (should_run)
	{
		sockaddr_storage client_address = {};

		std::mutex m1, m2;

		m1.lock();
		const sockets::sock new_client_data_socket = accept(listen_socket, (sockaddr*)&client_address,
			&socket_address_storage_size);
		m1.unlock();
		
		if (new_client_data_socket == sockets::BAD_SOCKET)
		{
			continue;
		}

		print_client_information(client_address, socket_address_storage_size);

		auto respond = [=]()
		{
			RespondToClientOrBrowser(new_client_data_socket);
		};

		m2.lock();
		std::thread t(respond);
		m2.unlock();
		t.detach();
	}

	sockets::close_socket(listen_socket);

	return 0;
}

namespace
{
	void sendToBrowserAndGetFromBrowser(sockets::sock writer)
	{
		std::array<char, 8096> receive_buffer{};
		int bytes_received = 0;

		while (true)
		{
			std::mutex m;

			m.lock();
			if ((bytes_received = recv(writer, receive_buffer.data(), receive_buffer.size(), 0)) != 0)
			{
				std::string temp(receive_buffer.data());
				std::string msg;

				if (bytes_received < 0)
					break;

				if (*std::find(temp.begin(), temp.end(), '{') == '{' && *std::find(temp.begin(), temp.end(), '}') == '}')
				{
					std::copy(std::find(temp.begin(), temp.end(), '{') + 1, std::find(temp.begin(), temp.end(), '}'), std::back_inserter(msg));

					std::for_each(browser_socket.begin(), browser_socket.end(), [&](sockets::sock browsers)
					{
						std::mutex m1;
						m1.lock();
						send(browsers, msg.data(), msg.size(), 0);
						m1.unlock();
						logInfo += msg;
					});
				}
				m.unlock();
				msg.clear();
			}
			receive_buffer[bytes_received] = 0;
		}
	}

	void print_client_information(sockaddr_storage client_address, socklen_t socket_address_storage_size)
	{
		constexpr int NameBufferLength = 512;
		std::array<char, NameBufferLength> client_hostname{};
		std::array<char, NameBufferLength> client_port{};
		const auto psocketaddress_information = reinterpret_cast<sockaddr*>(&client_address);

		getnameinfo(psocketaddress_information, socket_address_storage_size, client_hostname.data(),
			NameBufferLength, client_port.data(), NameBufferLength, NI_NUMERICHOST);

		std::cout << "Connected to (" << client_hostname.data() << ", " << client_port.data() << ") / ";

		getnameinfo(psocketaddress_information, socket_address_storage_size, client_hostname.data(),
			NameBufferLength, client_port.data(), NameBufferLength, NI_NUMERICSERV);

		std::cout << "(" << client_hostname.data() << ", " << client_port.data() << ")\n\n";
	}

	void RespondToClientOrBrowser(sockets::sock conneted_socket)
	{
		std::array<char, 8069> classify{};
		std::mutex m1, m2;
		m1.lock();
		recv(conneted_socket, classify.data(), classify.size(), 0);
		m1.unlock();
		std::string writer = "writer";
		std::string welcome;

		std::string temp(classify.data());
		if (temp == "browser")
		{
			browser_socket.push_back(conneted_socket);
			m2.lock();
			send(conneted_socket, logInfo.data(), logInfo.size(), 0);
			m2.unlock();
		}
		else
		{
			std::for_each(browser_socket.begin(), browser_socket.end(), [&](sockets::sock browser)
			{
				std::mutex m;
				m.lock();
				send(browser, temp.data(), temp.size(), 0);
				logInfo += temp;
				m.unlock();
			});
			writer_socket.push_back(conneted_socket);
			sendToBrowserAndGetFromBrowser(conneted_socket);
		}
	}
}