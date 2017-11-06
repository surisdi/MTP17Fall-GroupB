//Socket.hpp

#ifndef SOCKET_HPP
#define SOCKET_HPP

#define TCP_PORT 2222
#define PAYLOAD 25


#include "RF24/RF24.h"


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
    SocketTCP(bool mode, char* ip, bool isServer);
    
    virtual bool read_blocking(char *buff, int len) override;
    
    virtual bool read_non_blocking(char *buff, int len, int timeout, int *timeout_info) override;
    
    virtual bool write_socket(const char *buff, int len, int mode) override;
    
    virtual ~SocketTCP();
};

/***************** Derived Class Socket Radio *****************/

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

#endif
