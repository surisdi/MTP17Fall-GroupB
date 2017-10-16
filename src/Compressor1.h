/*
 * Compressor1.h
 *
 *  Created on: 16 oct. 2017
 *      Author: genis
 */

#ifndef COMPRESSOR1_H_
#define COMPRESSOR1_H_

#include "Compressor.h"

class Compressor1: public Compressor {

public:
	Compressor1(float, int);

	virtual void compress(char *, char*);

	virtual void decompress(char *, char*);

	virtual ~Compressor1();
};

#endif /* COMPRESSOR1_H_ */
