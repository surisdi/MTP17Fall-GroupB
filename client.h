#ifndef CLIENT_H_   /* Include guard */
#define CLIENT_H_


int initiateSocket(char *ip, int port);

//void radio (char *text, int len, int sockfd);

int send_channel(char *sequence, int len, int socket);

#endif