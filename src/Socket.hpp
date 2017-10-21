//Socket.hpp

#ifndef SOCKET_HPP
#define SOCKET_HPP

#define TCP_PORT 2222
#define PAYLOAD 25

/***************** Base Class Socket *****************/

class Socket {
protected:
	int socket_id;
	int bufferSize;

public:
	Socket(int bSize);

	virtual int read_blocking(char *buff, int timeout) = 0;

	virtual int read_non_blocking(char *buff) = 0;

	virtual int write_socket(const char *buff) = 0;

	virtual ~Socket();
};

/***************** Derived Class SocketTCP *****************/

class SocketTCP: public Socket {
public:
	SocketTCP(char* ip, bool isServer, int bSize);

	virtual int read_blocking(char *buff, int timeout) override;

	virtual int read_non_blocking(char *buff) override;

	virtual int write_socket(const char *buff) override;

	virtual ~SocketTCP();

private:
	int read_with_timeout(int socket, int timeout, char *packet, int code_length, int *ret);
};

#endif /* SOCKET_H_ */
