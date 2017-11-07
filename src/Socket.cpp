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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>

#include <iostream>

#include <netdb.h>

#define PORT1 8888
#define PORT2 8889
#define BUFLEN 512

//#include "RF24/RF24.h"


/***************** Base Class Socket *****************/
Socket::Socket(bool mode) {
    this->mode = mode;
}
Socket::~Socket(){}

/***************** Derived Class Socketradio *****************/
/*
SocketRadio::SocketRadio(bool mode): Socket(mode){
    // Setup for GPIO 15 CE and CE0 CSN with SPI Speed @ 8Mhz 
    radio_sender = new RF24(25, 0);
    radio_ack = new RF24(25, 1);
    
    // radio_sender pipe addresses for the 2 nodes to communicate. 
    const uint64_t addresses[2] = { 0xABCDABCD71LL, 0x544d52687CLL };
    
    // Setup the spec the radio for the data tx
    radio_sender->begin();
    radio_sender->setChannel(97);
    radio_sender->setPALevel(RF24_PA_MIN);
    radio_sender->setDataRate(RF24_2MBPS);
    radio_sender->setAutoAck(0);
    //radio_sender->setRetries(2,15);
    radio_sender->setCRCLength(RF24_CRC_8);
    
    radio_sender->printDetails();

    // Setup the spec the radio for the ack tx
    radio_ack->begin();
    radio_ack->setChannel(97);
    radio_ack->setPALevel(RF24_PA_MIN);
    radio_ack->setDataRate(RF24_2MBPS);
    radio_ack->setAutoAck(0);
    //radio_sender->setRetries(2,15);
    radio_ack->setCRCLength(RF24_CRC_8);
    
    radio_ack->printDetails();
    
    if(mode){
        radio_sender->openWritingPipe(addresses[1]);
        radio_ack->openReadingPipe(1, addresses[0]);
    }else{
        radio_ack->openWritingPipe(addresses[0]);
        radio_sender->openReadingPipe(1,addresses[1]);
    }
}


bool SocketRadio::read_blocking(char *buff, int len){

    radio_sender->startListening();
    while(!radio_sender->available()){
     	// sleep
    }
    std::cout << "Is available\n" << std::endl;
    while(radio_sender->available()){
	radio_sender->read(buff, len); // Per aqui pot fallar
    }

    return 1;
}

bool SocketRadio::read_non_blocking(char *buff, int len, int timeout, int *timeout_info){
    radio_ack->startListening();
    unsigned long startTime = millis();
    *timeout_info = 0;
    while(!radio_ack->available()){
        if(millis()-startTime > timeout){
            return -1;
        }
        //sleep(1);
    }
    while(radio_ack->available()){
        *timeout_info = 1;
        radio_ack->read(buff, len);
    }
    return 1;
}

bool SocketRadio::write_socket(const char *buff, int len, int mode){
    if(mode == 0){
        radio_sender->stopListening();
        // Open writing pipe
        //TODO check the address for possible problems with the role

        int result = (int) radio_sender->write(buff, len);
        radio_sender->txStandBy();
    }
    else{
        radio_ack->stopListening();
        int result = (int) radio_ack->write(buff, len);
        radio_ack->txStandBy();
    }
}


SocketRadio::~SocketRadio() {
    
}

*/

/***************** Derived Class SocketTCP *****************/

SocketTCP::SocketTCP(bool mode, char* ip): Socket(mode){
    bool isServer = mode;
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

bool SocketTCP::read_non_blocking(char* buff, int len, int timeout, int *timeout_info){
    
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
            return read(socket_id, buff, len);
    }
    
    return 0;
}

bool SocketTCP::read_blocking(char* buff, int len){
    return read (socket_id, buff, len);
}

bool SocketTCP::write_socket(const char* buff, int len, int mode){
    int n = write(socket_id, buff, len);
    if (n < 0){
        printf("ERROR writing to socket");
        return 0;
    }
    
    return 1;
}

SocketTCP::~SocketTCP() {
    std::cout << "SocketTCP destroyed... " << std::endl;
}

/***************** Derived Class SocketUDP *****************/

SocketUDP::SocketUDP(bool mode, char* ip): Socket(mode){

    int PORT_TX=PORT1, PORT_RX=PORT2;
    if (mode){ //tx
        PORT_TX=PORT2;
        PORT_RX=PORT1;
    }

    int s_rx, s_tx;
    struct sockaddr_in si_other_tx, si_other_rx, si_me_rx;

    // Creacio servidor
    if((s_rx=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1){
        std::cout << "Error in socket" << std::endl;
    }

    memset((char*) &si_me_rx, 0, sizeof(si_me_rx));
    si_me_rx.sin_family = AF_INET;
    si_me_rx.sin_port = htons(PORT_RX);
    si_me_rx.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(s_rx, (sockaddr*)&si_me_rx, sizeof(si_me_rx))==-1){
        std::cout << "Error in bind" << std::endl;
    }

    // Creacio client
    if((s_tx=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1){
        std::cout << "Error in socket" << std::endl;
    }
    memset((char*) &si_other_tx, 0, sizeof(si_other_tx));
    si_other_tx.sin_family = AF_INET;
    si_other_tx.sin_port = htons(PORT_TX);

    if (inet_aton(ip, &si_other_tx.sin_addr)==0){
        std::cout << "Error" << std::endl;
    }

    si_me_rx_ = si_me_rx;
    si_other_tx_ = si_other_tx;
    si_other_rx_ = si_other_rx;
    socket_id_tx = s_tx;
    socket_id_rx = s_rx;

}

bool SocketUDP::read_non_blocking(char* buff, int len, int timeout, int *timeout_info){
    struct pollfd fds; // Polling system for timeouts
    unsigned int slen=sizeof(si_other_rx_);
    fds.fd = socket_id_rx;
    fds.events = POLLIN;
    
    *timeout_info = poll(&fds, 1, timeout);
    //std::cout << "Timeout info: " << *timeout_info << std::endl;
    int n = -2;
    switch(*timeout_info) {
        case -1:
            //std::cout << "Error Poll " << std::endl;
            break;
        case 0:
            //std::cout << "Timeout Expired " << std::endl;
            break;
        default:
            return recvfrom (socket_id_rx, buff, len, 0, (sockaddr*)&si_other_rx_, &slen);
    }
    
    return 0;
}

bool SocketUDP::read_blocking(char* buff, int len){
    unsigned int slen=sizeof(si_other_rx_);
    std::cout << "Read blocking..." << std::endl;
    return recvfrom (socket_id_rx, buff, len, 0, (sockaddr*)&si_other_rx_, &slen);
}

bool SocketUDP::write_socket(const char* buff, int len, int mode){
    unsigned int slen=sizeof(si_other_tx_);
    std::cout << "Write to socket " <<  socket_id_tx << std::endl;
    int n = sendto(socket_id_tx, buff, len, 0, (sockaddr*)&si_other_tx_, slen);
    if (n < 0){
        std::cout << "Error writing to socket" << std::endl;
        return 0;
    }
    std::cout << "Written ..." << std::endl;
    
    return 1;
}

SocketUDP::~SocketUDP() {
    std::cout << "SocketTCP destroyed... " << std::endl;
    close(socket_id_tx);
    close(socket_id_rx);
}

