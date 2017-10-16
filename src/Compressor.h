/*
 * Compressor.h
 *
 *  Created on: 16 oct. 2017
 *      Author: genis
 */

#ifndef COMPRESSOR_H_
#define COMPRESSOR_H_

class Compressor {

protected:
	float compressionRate;
	int partitions;

public:
	Compressor(float, int);

	virtual void compress(char *, char*) = 0;

	virtual void decompress(char *, char*) = 0;

	virtual ~Compressor();
};

#endif /* COMPRESSOR_H_ */
