//Socket.hpp

#ifndef SOCKET_H_
#define SOCKET_H_

#define TCP_PORT 2222
#define PAYLOAD 25

/***************** Base Class Socket *****************/

class Socket {
protected:
	int socket_id;

public:
	Socket();
	
	virtual int read_blocking(char *buff, int len) = 0;

	virtual int read_non_blocking(char *buff, int len, int timeout, int *timeout_info) = 0;

	virtual int write_socket(const char *buff, int len) = 0;

	virtual ~Socket();
};

/***************** Derived Class SocketTCP *****************/

class SocketTCP: public Socket {
public:
	SocketTCP(char* ip, bool isServer);

	virtual int read_blocking(char *buff, int len) override;

	virtual int read_non_blocking(char *buff, int len, int timeout, int *timeout_info) override;

	virtual int write_socket(const char *buff, int len) override;

	virtual ~SocketTCP();
};

#endif /* SOCKET_H_ */
