// Socket.hpp

#ifndef SOCKET_H
#define SOCKET_H

#define TCP_PORT 2222
#define PAYLOAD 25

/**********************************************************************/

// Base class
class Socket {
	public:
		Socket(int bSize){
			bufferSize = bSize;
		}	

		// pure virtual function
		virtual void write(char* buff) = 0;
		virtual int read_blocking(char* buff, int timeout) = 0;
		virtual int read_non_blocking(char* buff) = 0;
	private:
		int socket;
		int bufferSize;
}


/*********************************************************************/

// TCP Socket
class SocketTCP: public Socket{

	SocketTCP(char* ip, int port, bool isServer, int bSize); 
	void write(char* buff);
	void read_blocking(char*  buff, int timeout);
	int read_non_blocking(char* buff);
}

/************************************************************************/

// Radio Socket

#endif
