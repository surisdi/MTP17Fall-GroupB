#include "comms.h"

#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>

#include "poll.h"

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

//TO-DO It extracts the label from the ACK
int get_ack_label(char *r_ack) {
    return (int) (r_ack[L_ACK]);
}

int send_channel(char *sequence, int len, int socket){
	// Simulate noisy channel
	float prob_error_bit = 0.01;
	float prob_lost = 0.5;
	bsc(sequence, len, prob_error_bit);

	// Simulate random drop (loss) of packets with prob 0.2
    double w = (double)rand()/ (double) RAND_MAX;
    if (w > prob_lost) { 
        printf("\n *** Packet will be lost *** \n");
        // usleep(2000000);
        // continue;
    }
    else{
    	send(sequence, len, socket);
    }
}

int read_with_timeout(int socket, int timeout, char *packet, int code_length, int *ret){

	struct pollfd fds; // Polling system for timeouts
    fds.fd = socket;
    fds.events = POLLIN;

    *ret = poll(&fds, 1, timeout);
    printf("\n RET %i\n", *ret);
    int n = -2;
    switch(*ret) {
        case -1:
            printf("\n Error Poll \n");
            break;
        case 0:
            printf("\n Timeout \n");
            break;
        default:
            n = receive(packet, code_length, fds.fd);
    }
    return n;
        
}

void sendAck(int socket, bool is_ack){
	if (is_ack)
		send(ack, L_ACK, socket);
	
	else
		send(nack, L_ACK, socket);
}

int send (char *text, int len, int sockfd) {
	return write(sockfd, text, len);
}

int receive(char *buffer, int len, int sockfd) {
	return read(sockfd, buffer, len);
}

