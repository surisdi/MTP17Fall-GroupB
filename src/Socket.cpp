//Socket.cpp

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

#include "RF24/RF24.h"


/***************** Base Class Socket *****************/
Socket::Socket(){}
Socket::~Socket(){}

/***************** Derived Class SocketTCP *****************/

SocketRadio::SocketRadio(): Socket(){
	// Setup for GPIO 15 CE and CE0 CSN with SPI Speed @ 8Mhz 
	radio = new RF24(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

	// Radio pipe addresses for the 2 nodes to communicate. 
	const uint64_t addresses[2] = { 0xABCDABCD71LL, 0x544d52687CLL };
	
	radio->begin();
	radio->setChannel(1);
	radio->setPALevel(RF24_PA_MIN);
	radio->setDataRate(RF24_1MBPS);
	radio->setAutoAck(1);
	radio->setRetries(2,15);
	radio->setCRCLength(RF24_CRC_8);
	// radio->printDetails();
	
	bool is_transmiter = true;
	bool is_receiver = false;
	
	if(is_transmiter){
		radio->openWritingPipe(addresses[0]);
		radio->openReadingPipe(1, addresses[1]);
		radio->stopListening();
	}
	if(is_receiver){
		radio->openWritingPipe(addresses[1]);
		radio->openReadingPipe(1,addresses[0]);
		radio->startListening();
	}
}


int SocketRadio::read_blocking(char *buff, int len){
	radio->startListening();
	while(!radio->available()){
		sleep(1);
	}
	while(radio->available()){
		radio->read(&buff, len);
	}
	return 0;
}

int SocketRadio::read_non_blocking(char *buff, int len, int timeout, int *timeout_info){
	radio->startListening();
	unsigned long startTime = millis();
	*timeout_info = 0;
	while(!radio->available()){
		if(millis()-startTime > timeout){
			*timeout_info = 1;
			break;
		}
		sleep(1);
	}
	while(radio->available()){
		radio->read(&buff, len);
	}
	return 0;
}

int SocketRadio::write_socket(const char *buff, int len){
	radio->stopListening();
	// Open writing pipe
	//TODO check the address for possible problems with the role
	return radio->writeFast(&buff, len);
}

SocketRadio::~SocketRadio() {
	
}



/***************** Derived Class SocketTCP *****************/

SocketTCP::SocketTCP(char* ip, bool isServer): Socket(){
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
     	}else{
     		printf("You are connected \n");
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

int SocketTCP::read_non_blocking(char* buff, int len, int timeout, int *timeout_info){

	struct pollfd fds; // Polling system for timeouts
    fds.fd = socket_id;
    fds.events = POLLIN;

    *timeout_info = poll(&fds, 1, timeout);
    printf("\n Timeout info: %i\n", *timeout_info);
    int n = -2;
    switch(*timeout_info) {
        case -1:
            printf("\n Error Poll \n");
            break;
        case 0:
            printf("\n Timeout Expired \n");
            break;
        default:
            n = read(socket_id, buff, len);
    }

    return 0;
}

int SocketTCP::read_blocking(char* buff, int len){
	return read (socket_id, buff, len);
}

int SocketTCP::write_socket(const char* buff, int len){
	int n = write(socket_id, buff, len);
    if (n < 0)
		printf("ERROR writing to socket");

	return n;
}

SocketTCP::~SocketTCP() {
	std::cout << "SocketTCP destroyed... " << std::endl;
}

