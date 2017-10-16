/*
 * Compressor.cpp
 *
 *  Created on: 16 oct. 2017
 *      Author: genis
 */

#include <iostream>
#include "Compressor.h"

Compressor::Compressor(float cmpRate, int part):
compressionRate(cmpRate), partitions(part)
{
}

Compressor::~Compressor()
{
}

