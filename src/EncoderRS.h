/*
 * EncoderRS.h
 *
 *  Created on: 16 oct. 2017
 *      Author: genis
 */

#ifndef ENCODERRS_H_
#define ENCODERRS_H_

#include "Encoder.h"

class EncoderRS: public Encoder {
public:
	EncoderRS(int, int);

	virtual int encode(char *, char *);

	virtual int decode(char *, char *);

	virtual ~EncoderRS();
};

#endif /* ENCODERRS_H_ */
