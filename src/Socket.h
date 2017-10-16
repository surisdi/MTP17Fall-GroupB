/*
 * Socket.h
 *
 *  Created on: 16 oct. 2017
 *      Author: genis
 */

#ifndef SOCKET_H_
#define SOCKET_H_

class Socket {
public:
	Socket();

	virtual int read_blocking(char *) = 0;

	virtual int read_non_blocking(char *) = 0;

	virtual int write(const char *) = 0;

	virtual ~Socket();
};

#endif /* SOCKET_H_ */
