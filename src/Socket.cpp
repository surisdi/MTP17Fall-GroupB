//Socket.cpp

#include "Socket.hpp"
#include "utils.hpp"

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


/***************** Base Class Socket *****************/
Socket::Socket(bool mode): mode(mode),
socket_id(0){}

Socket::~Socket(){}

/***************** Derived Class Socketradio *****************/

SocketRadio::SocketRadio(bool mode): Socket(mode){
    // Setup for GPIO 15 CE and CE0 CSN with SPI Speed @ 8Mhz
    radio_sender = new RF24(25, 0);
    radio_ack = new RF24(24, 1);

    // radio_sender pipe addresses for the 2 nodes to communicate.
    const uint64_t addresses[2] = {0xABCDABCD71LL, 0x544d52687CLL};
    //const uint64_t addresses[4] = { 0xABCDABCD71LL, 0xBACDBACD71LL, 0x533d52687CLL, 0x544d52687CLL};

    // Setup the spec the radio for the data tx
    radio_sender->begin();
    radio_sender->setChannel(90);
    radio_sender->setPALevel(RF24_PA_MIN);
    radio_sender->setDataRate(RF24_1MBPS);
    radio_sender->setAutoAck(0);
    //radio_sender->setRetries(2,15);
    radio_sender->setCRCLength(RF24_CRC_8);
    radio_sender->printDetails();

    // Setup the spec the radio for the ack tx
    radio_ack->begin();
    radio_ack->setChannel(99);
    radio_ack->setPALevel(RF24_PA_MIN);
    radio_ack->setDataRate(RF24_1MBPS);
    //radio_ack->maskIRQ(1,1,0);
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

bool SocketRadio::read_blocking(byte *buff, int len){
    mtx_radio.lock();
    radio_sender->startListening();
    mtx_radio.unlock();

    bool avail=false;
    while(!avail){
    	mtx_radio.lock();
		avail=radio_sender->available();
		mtx_radio.unlock();
     	// sleep
    }
    COUT<< "Is available\n\n";

    mtx_radio.lock();
	radio_sender->read(buff, len); // Per aqui pot fallar
	mtx_radio.unlock();

    return 1;
}

bool SocketRadio::read_non_blocking(byte *buff, int len, int timeout, int *timeout_info){
    mtx_radio.lock();
    radio_ack->startListening();
    mtx_radio.unlock();

    unsigned long startTime = millis();
    *timeout_info = 0;
    bool avail=false;
    while(!avail){
    	mtx_radio.lock();
    	avail=radio_ack->available();
    	mtx_radio.unlock();
    	if(millis()-startTime > timeout){
    		return -1;
    	}
        //sleep(1);
    }
	*timeout_info = 1;
	mtx_radio.lock();
	radio_ack->read(buff, len);
	mtx_radio.unlock();

    return 1;
}

bool SocketRadio::write_socket(const byte *buff, int len, int mode){
    if(mode == 0){
    	mtx_radio.lock();
        radio_sender->stopListening();

        // Open writing pipe
        //TODO check the address for possible problems with the role

        int result = (int) radio_sender->write(buff, len);
        radio_sender->txStandBy();
        radio_ack->startListening();
        mtx_radio.unlock();
    }
    else{
    	mtx_radio.lock();
        radio_ack->stopListening();
        int result = (int) radio_ack->write(buff, len);
        radio_ack->txStandBy();
        mtx_radio.unlock();
    }
}


SocketRadio::~SocketRadio() {}


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
        COUT<< "Error in socket\n";
    }

    memset((char*) &si_me_rx, 0, sizeof(si_me_rx));
    si_me_rx.sin_family = AF_INET;
    si_me_rx.sin_port = htons(PORT_RX);
    si_me_rx.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(s_rx, (sockaddr*)&si_me_rx, sizeof(si_me_rx))==-1){
        COUT<< "Error in bind\n";
    }

    // Creacio client
    if((s_tx=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1){
        COUT<< "Error in socket\n";
    }
    memset((char*) &si_other_tx, 0, sizeof(si_other_tx));
    si_other_tx.sin_family = AF_INET;
    si_other_tx.sin_port = htons(PORT_TX);

    if (inet_aton(ip, &si_other_tx.sin_addr)==0){
        //COUT<< "Address is not valid\n";
        // Actually this will give always an error, because this function
        // transforms the address in format a.b.c.d to binary, and the 
        // address we pass is "localhost". Nothing to worry about.
    }

    si_me_rx_ = si_me_rx;
    si_other_tx_ = si_other_tx;
    si_other_rx_ = si_other_rx;
    socket_id_tx = s_tx;
    socket_id_rx = s_rx;

    COUT << "Socket UDP created\n";
}

bool SocketUDP::read_non_blocking(byte* buff, int len, int timeout, int *timeout_info){
    struct pollfd fds; // Polling system for timeouts
    unsigned int slen=sizeof(si_other_rx_);
    fds.fd = socket_id_rx;
    fds.events = POLLIN;
    
    *timeout_info = poll(&fds, 1, timeout);
    //COUT<< "Timeout info: " << *timeout_info << "\n";
    switch(*timeout_info) {
        case -1:
            //COUT<< "Error Poll \n";
            break;
        case 0:
            //COUT<< "Timeout Expired \n";
            break;
        default:
            return recvfrom (socket_id_rx, buff, len, 0, (sockaddr*)&si_other_rx_, &slen);
    }
    
    return 0;
}

bool SocketUDP::read_blocking(byte* buff, int len){
    unsigned int slen=sizeof(si_other_rx_);
    COUT<< "Read blocking...\n";
    return recvfrom (socket_id_rx, buff, len, 0, (sockaddr*)&si_other_rx_, &slen);
}

bool SocketUDP::write_socket(const byte* buff, int len, int mode){
    unsigned int slen=sizeof(si_other_tx_);
    int n = sendto(socket_id_tx, buff, len, 0, (sockaddr*)&si_other_tx_, slen);
    if (n < 0){
        COUT<< "Error writing to socket\n";
        return 0;
    }
    
    return 1;
}

SocketUDP::~SocketUDP() {
    COUT<< "SocketTCP destroyed... \n";
    close(socket_id_tx);
    close(socket_id_rx);
}

