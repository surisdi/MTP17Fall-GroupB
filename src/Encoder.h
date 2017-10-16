/*
 * Encoder.h
 *
 *  Created on: 16 oct. 2017
 *      Author: genis
 */

#ifndef ENCODER_H_
#define ENCODER_H_

class Encoder {

protected:
	int code_length;
	int redundancy;

public:
	Encoder(int, int);

	virtual int encode(char *, char *) = 0;

	virtual int decode(char *, char *) = 0;

	virtual ~Encoder();
};

#endif /* ENCODER_H_ */
