// comms.cpp

#include "comms.hpp"

namespace comms
{

	void create_ack(){}

	int is_ack(char *r_ack) {

	    int is_ack = 1, is_nack = 1;
	    int i = 0;

		while (i < L_ACK && (is_ack == 1 || is_nack == 1)) {

			if (r_ack[i] != ack[i])
			    is_ack = 0;

			if(r_ack[i] != nack[i])
			    is_nack = 0;

			i++;
	    }

	    return is_ack - is_nack;
	}

	int crete_packet(){
		return 0;
	}
}
