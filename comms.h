#ifndef COMMS_H_   /* Include guard */
#define COMMS_H_

void bsc(char *sequence, int len, float prob);

void packet_builder(char *packet, char *data, char *fec, int pay_len, int fec_len, int flag);

int isAck(char *r_ack);

int get_ack_label(char *r_ack);

void sendAck(int socket, bool is_ack);

int read_with_timeout(int socket, int timeout, char *packet, int code_length, int *ret);

int send (char *text, int len, int sockfd);

int receive(char *buffer, int len, int sockfd);

#endif
