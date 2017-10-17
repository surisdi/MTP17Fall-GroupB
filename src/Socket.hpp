//Socket.hpp

#ifndef SOCKET_H_
#define SOCKET_H_

/***************** Base Class Socket *****************/

class Socket {
protected:
	int socket_id;
	int bufferSize;

public:
	Socket(int bSize);

	virtual int read_blocking(char *buff, int timeout) = 0;

	virtual int read_non_blocking(char *buff) = 0;

	virtual int write(const char *buff) = 0;

	virtual ~Socket();
};

/***************** Derived Class SocketTCP *****************/

class SocketTCP: public Socket {
public:
	SocketTCP(int bSize);

	virtual int read_blocking(char *buff, int timeout) override;

	virtual int read_non_blocking(char *buff) override;

	virtual int write(const char *buff) override;

	virtual ~SocketTCP();
};

#endif /* SOCKET_H_ */