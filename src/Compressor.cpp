//Compressor.cpp

#include <iostream>
#include "Compressor.hpp"
#include "utils.hpp"

/***************** Base Class Compressor *****************/

Compressor::Compressor(float cmpRate, int n_part):
compressionRate(cmpRate), n_partitions(n_part) {}

Compressor::~Compressor() {}


/***************** Derived Class Compressor1 *****************/

Compressor1::Compressor1(float cmpRate, int n_part):
Compressor(cmpRate, n_part)
{
	COUT<< "Compressor1 created...\n";
}

void Compressor1::compress(const char *input, char *output)
{
	COUT<< "File compressed with compressor 1 \n";
}

void Compressor1::decompress(const char *input, char *output)
{
	COUT<< "File decompressed with compressor 1 \n";
}

Compressor1::~Compressor1() {
	COUT<< "Compressor1 destroyed...\n";
}

