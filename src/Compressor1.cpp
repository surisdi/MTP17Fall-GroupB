/*
 * Compressor1.cpp
 *
 *  Created on: 16 oct. 2017
 *      Author: genis
 */

#include <iostream>
#include "Compressor1.h"

Compressor1::Compressor1(float cmpRate, int part):
Compressor(cmpRate, part)
{
	std::cout << "Compressor1 created..." << std::endl;

}

void Compressor1::compress(char *full_text, char *compress_text)
{
	std::cout << "File compressed with compressor 1 " << std::endl;
}

void Compressor1::decompress(char *full_text, char *compress_text)
{
	std::cout << "File decompressed with compressor 1 " << std::endl;
}

Compressor1::~Compressor1() {
	// TODO Auto-generated destructor stub
}

