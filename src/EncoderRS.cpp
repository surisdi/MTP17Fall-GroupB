/*
 * EncoderRS.cpp
 *
 *  Created on: 16 oct. 2017
 *      Author: genis
 */

#include <iostream>
#include "EncoderRS.h"

EncoderRS::EncoderRS(int code_l, int redun):
Encoder(code_l, redun)
{
	std::cout << "Reed Solocom encoder created..." << std::endl;

}

int EncoderRS::encode(char *packet, char *code)
{
	std::cout << "RSEncoding..." << std::endl;
	return 0;
}

int EncoderRS::decode(char *packet, char *code)
{
	std::cout << "RSDecoding..." << std::endl;
	return 0;
}

EncoderRS::~EncoderRS() {
	std::cout << "EncodeRS destroyed..." << std::endl;
}

