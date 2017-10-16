// comms.hpp

#define L_ACK 4
char ack[4] = {'G','O','O','D'};
char nack[4] = {'B', 'A','A','D'};

namespace comms{
	void create_ack();
	int is_ack(char* r_ack);
	int crete_packet();
}