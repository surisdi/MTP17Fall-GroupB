// Socket.h

#ifndef SOCKET_H
#define SOCKET_H

class Socket{

public:
	virtual int read_blocking (char* buff);
	virtual int read_non_blocking (char* buff);
	virtual int writeBuff (char* buff);
};

#endif
