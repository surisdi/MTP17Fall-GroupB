#ifndef SERVER_H_   /* Include guard */
#define SERVER_H_


int initiateServer(int port);

int readFromSocket (char *buffer, int len, int newsockfd);

int writeFromSocket (char* text, int len, int newsockfd);


#endif