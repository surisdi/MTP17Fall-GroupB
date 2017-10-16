/*
 * Protocol.cpp
 *
 *  Created on: 16 oct. 2017
 *      Author: genis
 */

#include <iostream>
#include "Protocol.h"

Protocol::Protocol(Compressor *comp, Encoder *enc, Socket *sck):
compressor(comp),
encoder(enc),
socket(sck)
{
}

Protocol::~Protocol() {
	// TODO Auto-generated destructor stub
}

