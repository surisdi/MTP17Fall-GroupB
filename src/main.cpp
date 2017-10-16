//============================================================================
// Name        : main.cpp
// Author      : Genis Floriach
// Version     :
// Copyright   : 
//============================================================================

#include <iostream>

#include "StopWait.h"
#include "EncoderRS.h"
#include "Compressor1.h"
#include "SocketTCP.h"

using namespace std;

int main() {

	Compressor1 compress(3.0, 4);
	EncoderRS rs(4, 3);
	SocketTCP mysocket;

	StopWait protocol(&compress, &rs, &mysocket);
	protocol.send_text(0);

	return 0;
}
