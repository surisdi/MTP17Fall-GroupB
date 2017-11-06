//Socket.hpp

#ifndef SOCKET_HPP
#define SOCKET_HPP

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


#define TCP_PORT 2222
#define PAYLOAD 25

//#include "RF24/RF24.h"


/***************** Base Class Socket *****************/

class Socket {
protected:
    int socket_id;
    bool mode;
    
public:
    Socket(bool mode);
    
    virtual bool read_blocking(char *buff, int len) = 0;
    
    virtual bool read_non_blocking(char *buff, int len, int timeout, int *timeout_info) = 0;
    
    virtual bool write_socket(const char *buff, int len, int mode) = 0;
    
    virtual ~Socket();
};

/***************** Derived Class SocketTCP *****************/

class SocketTCP: public Socket {
public:
    SocketTCP(bool mode, char* ip);
    
    virtual bool read_blocking(char *buff, int len) override;
    
    virtual bool read_non_blocking(char *buff, int len, int timeout, int *timeout_info) override;
    
    virtual bool write_socket(const char *buff, int len, int mode) override;
    
    virtual ~SocketTCP();
};

/***************** Derived Class SocketUDP *****************/

class SocketUDP: public Socket {
public:
    SocketUDP(bool mode, char* ip);
    
    virtual bool read_blocking(char *buff, int len) override;
    
    virtual bool read_non_blocking(char *buff, int len, int timeout, int *timeout_info) override;
    
    virtual bool write_socket(const char *buff, int len, int mode) override;
    
    virtual ~SocketUDP();

private:

    struct sockaddr_in si_other_tx_, si_other_rx_, si_me_rx_;
    int socket_id_rx, socket_id_tx;
};


/***************** Derived Class Socket Radio *****************/
/*
class SocketRadio: public Socket {
    
private:
    RF24* radio_sender;
    RF24* radio_ack;
public:
    SocketRadio(bool mode);
    
    virtual bool read_blocking(char *buff, int len) override;
    
    virtual bool read_non_blocking(char *buff, int len, int timeout, int *timeout_info) override;
    
    virtual bool write_socket(const char *buff, int len, int mode) override;
    
    virtual ~SocketRadio();
    
};
*/
#endif
