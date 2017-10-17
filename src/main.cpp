// main.cpp

#include <iostream>

#include "Protocol.hpp"
#include "Encoder.hpp"
#include "Compressor.hpp"
#include "Socket.hpp"

using namespace std;

int main() {

	Compressor1 compress(3.0, 4);
	EncoderRS rs(4, 3);
	SocketTCP mysocket(10);

	StopWait protocol(&compress, &rs, &mysocket);
	protocol.send_text(0);

	return 0;
}
