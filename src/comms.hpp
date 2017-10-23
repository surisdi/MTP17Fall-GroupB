// comms.hpp

#ifndef COMMS_HPP
#define COMMS_HPP

#define L_ACK 4

namespace comms{

	extern const int _code_l;
	extern const int _fec_l;
	extern const int _data_l;

	extern const int _ack_code_l;
	extern const int _ack_fec_l;
	extern const int _ack_data_l;

	extern const char ack[4];
	extern const char nack[4];


	void create_ack();
	int is_ack(char* r_ack);
	int crete_packet();
}

#endif
