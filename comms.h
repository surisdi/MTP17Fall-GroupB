#ifndef COMMS_H_   /* Include guard */
#define COMMS_H_

void bsc(char *sequence, int len, float prob);

void packet_builder(char *packet, char *data, char *fec, int pay_len, int fec_len, int flag);

int isAck(char *r_ack);

int get_ack_label(char *r_ack);

#endif
