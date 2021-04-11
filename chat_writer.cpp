// File Name: chat_writer.cpp
// Purpose:   Defines a networked client to handle an chat server,
// Language:  C++
// Platform:  Visual Studio 15.8.2 w/ C++17, g++ (Ubuntu 7.3.0-16ubuntu3) 7.3.0
// Author:    Kang Tae Wook
// Creation date: 2018/11/20

#include "sockets.h"
#ifdef _WIN32
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#else
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#endif

void send_Message(std::string name, sockets::sock socket);

int main(int argc, char const* argv[])
{
	const char* const server_host = argv[1];
	const char* const port = argv[2];
	std::string name;

	if (argc == 3)
	{
		std::cout << "Enter your nickname : ";
		std::getline(std::cin, name);
	}
	else if (argc != 4)
	{
		std::cerr << "usage: " << argv[0] << " <server ip address> <port or service name> <nickname>\n";
		return 1;
	}
	else if (argc == 4)
	{
		name = argv[3];
	}

	const sockets::sock client_socket = sockets::open_writer_socket(server_host, port);

	if (client_socket == sockets::BAD_SOCKET)
		return 1;

	std::mutex m;
	m.lock();
	std::thread t(send_Message, name, client_socket);
	m.unlock();
	t.join();

	return 0;
}

void send_Message(std::string name, sockets::sock socket)
{
	std::mutex m;

	std::string join_message = name + " has joined the room!!!!\n";
	m.lock();
	send(socket, join_message.data(), join_message.size(), 0);
	m.unlock();

	while (true)
	{
		std::mutex m2;
		std::string leave_msg = "{" + name + " has left the room......\n" + "}";
		std::string input_line;
		std::string indicate;
		std::string save_message;
		std::getline(std::cin, input_line);
		m2.lock();
		if (input_line == "quit" || input_line.empty())
		{
			send(socket, leave_msg.data(), leave_msg.size(), 0);
			m2.unlock();
			break;
		}

		indicate = "{" + name + "> ";
		save_message += indicate;
		save_message += input_line;
		save_message += "\n";
		save_message += "}";
		input_line.clear();

		std::string final_message(save_message);

		const int bytes_sent = send(socket, final_message.c_str(), final_message.size(), 0);
		m2.unlock();
		final_message.clear();
#ifdef _WIN32
		if (bytes_sent <= 0)
		{
			if (bytes_sent < 0)
			{
				std::cerr << "Server has been shutdown" << std::endl;
				final_message.clear();
			}
			break;
		}
#else
		if(bytes_sent < -1)
		{
			break;
		}
#endif
	}
	sockets::close_socket(socket);
}
