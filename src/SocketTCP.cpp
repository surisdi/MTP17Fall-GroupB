/*
 * SocketTCP.cpp
 *
 *  Created on: 16 oct. 2017
 *      Author: genis
 */

#include <iostream>
#include "SocketTCP.h"

SocketTCP::SocketTCP() {
	std::cout << "Socket TCP created... " << std::endl;
}

int SocketTCP::read_blocking(char *buffer)
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
	std::cout << "Socket TCP destroyed... " << std::endl;
}

