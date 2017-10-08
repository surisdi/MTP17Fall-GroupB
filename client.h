#ifndef CLIENT_H_   /* Include guard */
#define CLIENT_H_


int initiateSocket(char *ip, int port);

//void radio (char *text, int len, int sockfd);

int send (char *text, int len, int sockfd);

int receive(char *buffer, int len, int sockfd);

#endif