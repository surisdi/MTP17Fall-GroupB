// Socket.cpp

#include "Socket.hpp"

#include "poll.h"

#include <cstdio>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <strings.h>
#include <cstring>

#include <iostream>

#include <netdb.h>

/*********************************************************************/

// TCP Socket

SocketTCP::SocketTCP(char* ip, bool isServer, int bSize): Socket(bSize){
	int port = TCP_PORT;
	if (isServer){
		int sockfd, newsockfd, portno;
    	socklen_t clilen;
	    
	    struct sockaddr_in serv_addr, cli_addr;
	    int n;
	    std::cout << "Waiting for a connection " << std::endl;
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

     	socket_id = newsockfd;

     	//newsockfd contains the address of the client we are connected to
	}else{
		int sockfd, portno, n;

	    struct sockaddr_in serv_addr;
	    struct hostent *server;

	    portno = port;
	    std::cout << "Trying to connect to a server " << std::endl;
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

     	socket_id = sockfd;

	    //sockfd contains the serveraddress
     }
}

int SocketTCP::write_socket(char* buff){
	int n = write(socket_id, buff, bufferSize);
    if (n < 0)	
		printf("ERROR writing to socket");

	return n;	
}

int SocketTCP::read_blocking_socket(char* buff, int timeout){
		int n = -1;
	int ret = -1;
	if (timeout == -1){
		n = read (socket_id, buff, bufferSize);
 	}else{
        n = read_with_timeout(socket_id, timeout, buff, bufferSize, &ret);
 	}

 	if (n < 0) printf("ERROR reading from socket");
 	else if (n == 0) printf ("Connection closed");
 	
	if (ret == 0) printf ("TIMEOUT EXPIRED");

	return n;
}

int SocketTCP::read_non_blocking_socket(char* buff){
	return read (socket_id, buff, bufferSize);
}

int SocketTCP::read_with_timeout(int socket, int timeout, char *packet, int code_length, int *ret){

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
            n = read_non_blocking_socket(packet);
    }
    return n;
        
}
/************************************************************************/

// Radio Socket
