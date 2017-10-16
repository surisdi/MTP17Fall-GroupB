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
		virtual int write_socket(char* buff) = 0;
		virtual int read_blocking_socket(char* buff, int timeout) = 0;
		virtual int read_non_blocking_socket(char* buff) = 0;
	protected:
		int socket_id;
		int bufferSize;
};


/*********************************************************************/

// TCP Socket
class SocketTCP: public Socket{
	public:
		SocketTCP(char* ip, bool isServer, int bSize); 
		int write_socket(char* buff) override;
		int read_blocking_socket(char*  buff, int timeout) override;
		int read_non_blocking_socket(char* buff) override;
		int read_with_timeout(int socket, int timeout, char *packet, int code_length, int *ret);

};

/************************************************************************/

// Radio Socket

#endif
