//Socket.cpp

#include <iostream>
#include "Socket.hpp"

/***************** Base Class Socket *****************/

Socket::Socket(int bSize): bufferSize(bSize),
socket_id(0) {}

Socket::~Socket() {}


/***************** Derived Class SocketTCP *****************/

SocketTCP::SocketTCP(int bSize): Socket(bSize) {
	std::cout << "Socket TCP created... " << std::endl;
}

int SocketTCP::read_blocking(char *buffer, int timeout)
{
	std::cout << "TCP Reading Blocking... "  << std::endl;
	return 0;
}

int SocketTCP::read_non_blocking(char *buffer)
{
	std::cout << "TCP Reading non blocking..." << std::endl;
	return 0;
}

int SocketTCP::write(const char *buffer)
{
	std::cout << "TCP writing..." << std::endl;
	return 0;
}

SocketTCP::~SocketTCP() {
	std::cout << "SocketTCP destroyed... " << std::endl;
}

