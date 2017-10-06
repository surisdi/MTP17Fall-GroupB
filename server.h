#ifndef SERVER_H_   /* Include guard */
#define SERVER_H_


int initiateServer(int port);

char* readFromSocket (int newsockfd);

void writeFromSocket (char* text, int newsockfd);


#endif