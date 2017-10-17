//Encoder.cpp

#include <iostream>
#include "Encoder.hpp"

/***************** Base Class Compressor *****************/

Encoder::Encoder(int code_l, int redun):
code_length(code_l), redundancy(redun) {}

Encoder::~Encoder() {}

/***************** Derived Class Compressor1 *****************/

EncoderRS::EncoderRS(int code_l, int redun):
Encoder(code_l, redun)
{
	std::cout << "Reed Solomon encoder created..." << std::endl;
}

int EncoderRS::encode(const char *input, char *output)
{
	std::cout << "RSEncoding..." << std::endl;
	return 0;
}

int EncoderRS::decode(const char *input, char *output)
{
	std::cout << "RSDecoding..." << std::endl;
	return 0;
}

EncoderRS::~EncoderRS() {
	std::cout << "EncodeRS destroyed..." << std::endl;
}

