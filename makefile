CC = g++
CFLAGS = -O2 -Wall -Wextra -pedantic -pthread -std=c++17

all: chat_writer chat_server chat_browser

chat_writer: chat_writer.o sockets.o
	$(CC) $(CFLAGS) -o chat_writer chat_writer.o sockets.o

chat_server: chat_server.o sockets.o
	$(CC) $(CFLAGS) -o chat_server chat_server.o sockets.o
	
chat_browser: chat_browser.o chat_browser.o
	$(CC) $(CFLAGS) -o chat_browser chat_browser.o sockets.o	

chat_writer.o: chat_writer.cpp
	$(CC) $(CFLAGS) -c chat_writer.cpp

chat_browser.o: chat_browser.cpp
	$(CC) $(CFLAGS) -c chat_browser.cpp

chat_server.o: chat_server.cpp
	$(CC) $(CFLAGS) -c chat_server.cpp

sockets.o: sockets.cpp
	$(CC) $(CFLAGS) -c sockets.cpp

clean:
	rm -rf $(wildcard *.o)