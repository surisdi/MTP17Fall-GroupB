//Compressor.cpp

#include <iostream>
#include "Compressor.hpp"

/***************** Base Class Compressor *****************/

Compressor::Compressor(float cmpRate, int n_part):
compressionRate(cmpRate), n_partitions(n_part) {}

Compressor::~Compressor() {}


/***************** Derived Class Compressor1 *****************/

Compressor1::Compressor1(float cmpRate, int n_part):
Compressor(cmpRate, n_part)
{
	std::cout << "Compressor1 created..." << std::endl;
}

void Compressor1::compress(const char *input, char *output)
{
	std::cout << "File compressed with compressor 1 " << std::endl;
}

void Compressor1::decompress(const char *input, char *output)
{
	std::cout << "File decompressed with compressor 1 " << std::endl;
}

Compressor1::~Compressor1() {
	std::cout << "Compressor1 destroyed..." << std::endl;
}

