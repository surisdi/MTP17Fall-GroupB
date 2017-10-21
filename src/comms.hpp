// comms.hpp
#ifndef COMMS_HPP
#define COMMS_HPP

namespace comms{

	extern const int L_ACK;
	extern const char ack[4];
	extern const char nack[4];

	void create_ack();
	int is_ack(char* r_ack);
	int crete_packet();
};

#endif
