// Socket.cpp

#include "Socket.hpp"

#include <cstdio>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <strings.h>
#include <cstring>

#include <netdb.h>

/*********************************************************************/

// TCP Socket

SocketTCP::SocketTCP(char* ip, int port, bool isServer, int bSize): Socket(bSize){
	if (isServer){
		int sockfd, newsockfd, portno;
    	socklen_t clilen;
	    
	    struct sockaddr_in serv_addr, cli_addr;
	    int n;

	    sockfd = socket(AF_INET, SOCK_STREAM, 0);
	    if (sockfd < 0){
	       printf("ERROR opening socket");
	    }
	    bzero((char *) &serv_addr, sizeof(serv_addr));
	    portno = port;
	    serv_addr.sin_family = AF_INET;
	    serv_addr.sin_addr.s_addr = INADDR_ANY;
	    serv_addr.sin_port = htons(portno);
	    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
	             printf("ERROR on binding");
        }
	    listen(sockfd,5);
	    clilen = sizeof(cli_addr);
	    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	    if (newsockfd < 0){
	         printf("ERROR on accept");	
     	}		

     	socket = newsockfd;

     	//newsockfd contains the address of the client we are connected to
	}else{
		int sockfd, portno, n;

	    struct sockaddr_in serv_addr;
	    struct hostent *server;

	    portno = port;
	    sockfd = socket(AF_INET, SOCK_STREAM, 0);
	    if (sockfd < 0){
	        printf("ERROR opening socket");
	    }
	    server = gethostbyname(ip);
	    if (server == NULL) {
	        fprintf(stderr,"ERROR, no such host\n");
	        exit(0);
	    }
	    bzero((char *) &serv_addr, sizeof(serv_addr));
	    serv_addr.sin_family = AF_INET;
	    bcopy((char *)server->h_addr,
	         (char *)&serv_addr.sin_addr.s_addr,
	         server->h_length);
	    serv_addr.sin_port = htons(portno);
	    int info=connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
	    if (info < 0){
	        printf("ERROR connecting\n");
	    }
	    else{
	        printf("You are connected \n");
	    }

     	socket = sockfd;

	    //sockfd contains the serveraddress
     }
}

void SocketTCP::write(char* buff){
	int n = write(socket, buff, bufferSize);
    if (n < 0)	
		printf("ERROR writing to socket");

	return n;	
}

void SocketTCP::read_blocking(char* buff, int timeout){
		int n = -1;
	int ret = -1;
	if (timeout == -1){
		n = read (socket, buff, bufferSize);
 	}else{
        n = read_with_timeout(socket, timeout, buff, bufferSize, &ret);
 	}

 	if (n < 0) printf("ERROR reading from socket");
 	else if (n == 0) printf ("Connection closed");
 	
	if (ret == 0) printf ("TIMEOUT EXPIRED")

	return n;
}

int SocketTCP::Sread_non_blocking(char* buff){
	return read (socket, buff, bufferSize);
}


/************************************************************************/

// Radio Socket
