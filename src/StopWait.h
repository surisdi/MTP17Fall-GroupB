/*
 * StopWait.h
 *
 *  Created on: 16 oct. 2017
 *      Author: genis
 */

#ifndef STOPWAIT_H_
#define STOPWAIT_H_

#include "Protocol.h"

class StopWait: public Protocol {
public:
	StopWait(Compressor *comp, Encoder *enc, Socket *sck);
	virtual int send_text(const char*);
	virtual int receive_text();
	virtual ~StopWait();
};

#endif /* STOPWAIT_H_ */
