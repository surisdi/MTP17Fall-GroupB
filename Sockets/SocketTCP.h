//SocketTCP.h

#ifndef SOCKETTCP_H
#define SOCKETTCP_H

class SocketTCP{

private:
	int socketNumber;
	int bufferSize;

public:
	SocketTCP (char* ip, int port, bool isServer, int buffSize);
	int read_blocking (char* buff);
	int read_non_blocking (char* buff);
	int writeBuff (char* buff);
};

#endif
