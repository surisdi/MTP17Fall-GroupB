/*
 * Protocol.h
 *
 *  Created on: 16 oct. 2017
 *      Author: genis
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "Compressor.h"
#include "Socket.h"
#include "Encoder.h"

class Protocol {

protected:
	Compressor *compressor;
	Encoder *encoder;
	Socket *socket;

public:

	Protocol(Compressor *, Encoder *, Socket *);

	virtual int send_text(const char*) = 0;
	virtual int receive_text() = 0;

	virtual ~Protocol();
};

#endif /* PROTOCOL_H_ */
