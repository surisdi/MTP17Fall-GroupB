/*
 * SocketTCP.h
 *
 *  Created on: 16 oct. 2017
 *      Author: genis
 */

#ifndef SOCKETTCP_H_
#define SOCKETTCP_H_

#include "Socket.h"

class SocketTCP: public Socket {
public:
	SocketTCP();

	virtual int read_blocking(char *);

	virtual int read_non_blocking(char *);

	virtual int write(const char *);

	virtual ~SocketTCP();
};

#endif /* SOCKETTCP_H_ */
