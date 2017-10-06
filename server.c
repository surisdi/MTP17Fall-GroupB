/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include "server.h"

#include <cstdio>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <strings.h>
#include <cstring>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int initiateServer(int port)
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        printf("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = port;
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              printf("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) 
          printf("ERROR on accept");


	 return newsockfd;
}


char* readFromSocket (int newsockfd){
	printf ("Reading from socket\n");	
		
	char* buffer = new char[256];

	bzero(buffer,256);
	int n = read(newsockfd,buffer,255);
	if (n < 0) printf("ERROR reading from socket");

	return buffer;
}

void writeFromSocket (char* text, int newsockfd){
	
	
	int n = write(newsockfd,text, strlen (text));
	if (n < 0) printf("ERROR writing to socket");
}


int main (){
	int socket = initiateServer (5555);
	
	char* buffer = readFromSocket (socket);

	printf (buffer);

    writeFromSocket(buffer, socket);

	return 0;
}	

