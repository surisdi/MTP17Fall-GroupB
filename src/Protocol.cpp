
//Protocol.cpp

#include <iostream>
#include "Protocol.hpp"
#include "utils.hpp"

#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <bitset>

/***************** Base Class Protocol *****************/

Protocol::Protocol(Compressor *comp, Encoder *enc, Socket *sck):
compressor(comp),
encoder(enc),
socket(sck),
finished_protocol(false),
timer_running(false){}

int Protocol::isAck(const byte* r_ack) {
        //std::bitset<sizeof(char)> bs(r_ack);
    //Forgive me god, for i have sinned
	int n_ones = 0;

    //std::bitset<8> x(*r_ack);
    //COUT<< "ack packet: " << x << "\n";
    // The 5 lowest bits are for flags (FOR PACKET ID)
	for (int k = 2; k < 8; k++) {
		if(((*r_ack) & (0x01 << k)) != 0) {
			n_ones++;
		}
	}

	if (n_ones == 6) {
		return 1;
	}else{
		return 0;
	}
}

Protocol::~Protocol() {}


