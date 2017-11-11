//Compressor.cpp

#include <iostream>
#include "Compressor.hpp"
#include "zlib.h"

/***************** Base Class Compressor *****************/

Compressor::Compressor(int cmpRate):
compressionRate(cmpRate),
outputFile(NULL),
n_chunks(0),
sizeDataUncompressed(utils::CHUNK_SIZE){}

Compressor::~Compressor() {}


/***************** Derived Class Compressor1 *****************/

Compressor1::Compressor1(int cmpRate):
Compressor(cmpRate)
{
	COUT<< "Compressor1 created...\n";
}

int Compressor1::compressFile(char *file_name, Chunk *output, int *num_chunk)
{
	int len;
	unsigned char *buffer = (unsigned char *)utils::read_text(file_name, &len);
	int extra = ((len % utils::CHUNK_SIZE) != 0);

	n_chunks = len / utils::CHUNK_SIZE + extra;
	int counter = 0;

	int numbytes = 0;
	for(int i = 0; i < n_chunks-1; i++) {
		unsigned long *size = (unsigned long*)malloc(sizeof(long));
		*size = utils::sizeCompressed;
		unsigned char *dataCompressed = (unsigned char*)malloc(sizeof(char)*utils::sizeCompressed);
		compress2(dataCompressed, size, &buffer[counter], utils::CHUNK_SIZE, compressionRate);
		counter += utils::CHUNK_SIZE;

		listChunk[i].data = dataCompressed;
		listChunk[i].len = size;
		numbytes += *size;
	}

	unsigned long *size = (unsigned long*)malloc(sizeof(long));
	*size = utils::sizeCompressed;
	unsigned char *dataCompressed = (unsigned char*)malloc(utils::sizeCompressed);
	int z_result = compress2(dataCompressed, size, &buffer[counter], len-counter, compressionRate);

	listChunk[n_chunks-1].data = dataCompressed;
	listChunk[n_chunks-1].len = size;
	numbytes += *size;

	output = listChunk;
	*num_chunk = n_chunks;

	COUT << "Size Original file: " << len << "\n";
	COUT << "Size compressed file: " << numbytes << "\n";

	if(z_result >= 0)
		return len - counter;
	else
		return z_result;
}

int Compressor1::startDecompress(char *file_name)
{
	outputFile = fopen(file_name, "wb");
	return outputFile == NULL;
}

int Compressor1::decompressChunk(Chunk *input, int chunk_size, char *output)
{
	if(chunk_size < utils::CHUNK_SIZE)
		sizeDataUncompressed = chunk_size;

	int z_result = uncompress(dataUncompressed, &sizeDataUncompressed, input->data, *(input->len));

	if(z_result >= 0) {
		int ret = fwrite(dataUncompressed, sizeof(char), sizeDataUncompressed, outputFile);

		free(input->data);
		free(input->len);

		return ret;
	} else {
		return z_result;
	}
}

int Compressor1::closeDecompress()
{
	int check = fclose(outputFile);
	outputFile = NULL;

	return check;
}

Compressor1::~Compressor1() {
	for(int i = 0; i < n_chunks; i++) {
		free(listChunk[i].data);
		free(listChunk[i].len);
	}
}

