// comms.cpp

#include "comms.hpp"

namespace comms
{
	extern const char ack[4] = {'G','O','O','D'};
	extern const char nack[4] = {'B', 'A','A','D'};

	extern const int _code_l= 32;
	extern const int _fec_l = 6;
	extern const int _data_l = _code_l - _fec_l;

	extern const int _ack_code_l = 4;
	extern const int _ack_fec_l = 2;
	extern const int _ack_data_l = _ack_code_l - _ack_fec_l;

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
