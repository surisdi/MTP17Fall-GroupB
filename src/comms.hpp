// comms.hpp

#define L_ACK 4
char ack[4] = {'G','O','O','D'};
char nack[4] = {'B', 'A','A','D'};

namespace comms{

	const int code_length = 32;
	const int fec_length = 6;
	const int data_length = code_length - fec_length;

	const int ack_code_length = 4;
	const int ack_fec_length = 2;
	const int ack_data_length = ack_code_length - ack_fec_length;


	void create_ack();
	int is_ack(char* r_ack);
	int crete_packet();
}
