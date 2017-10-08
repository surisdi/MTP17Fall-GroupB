#include "comms.h"

#include <cstdio>
#include <cstdlib>

#define L_ACK 4

char ack[4] = {'G','O','O','D'};
char nack[4] = {'B', 'A','A','D'};

void bsc(char *sequence, int len, float prob)
{
	double w;
	int b;
	int coin;

	for (int i=0; i < len; i++) {
		for (b = 0; b < 8; b++) {
			w = (double)rand()/ (double) RAND_MAX;
			coin = w < prob;
			sequence[i] = sequence[i] ^  (coin << b);
		}
	}
}


// It builds the complete packet to be send (Adds Codes... padding...)
void packet_builder(char *packet, char *data, char *fec, int pay_len, int fec_len, int flag) {

	//AFEGIR PADDING!!
    int i;
    for (i = 0; i < pay_len; i++) { //Copy data into packet
		packet[i] = data[i];
    }

    for (i = 0; i < fec_len; i++) {
        packet[i + pay_len] = fec[i];
    }

    //packet[len] = (char) flag; // Add flag
}

// Check if is ACK or NACK: ACK=0  NACK=2 Garbage=1
int isAck(char *r_ack) {

    int check = 1, check2 = 1;
    int i = 0;

	while (i < L_ACK && (check == 1 || check2 == 1)) {

		if (r_ack[i] != ack[i])
		    check = 0;

		if(r_ack[i] != nack[i])
		    check2 = 0;
    }

    return check + check2;
}

//TO-DO It extracts the label from the ACK
int get_ack_label(char *r_ack) {
    return (int) (r_ack[L_ACK]);
}
