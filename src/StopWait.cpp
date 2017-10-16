/*
 * StopWait.cpp
 *
 *  Created on: 16 oct. 2017
 *      Author: genis
 */

#include <iostream>
#include "StopWait.h"

StopWait::StopWait(Compressor *comp, Encoder *enc, Socket *sck):
Protocol(comp, enc, sck)
{
	std::cout << "Stop and wait created" << std::endl;
}

int StopWait::receive_text()
{
	std::cout << "Receiving text..." << std::endl;
	return 0;
}

int StopWait::send_text(const char *text) {
	std::cout << "Sending text..." << std::endl;
	return 0;
}

StopWait::~StopWait() {
	std::cout << "Stop and Wait destroyed..." << std::endl;
}

