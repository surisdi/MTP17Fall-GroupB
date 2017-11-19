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
	COUT<< "Compressor1 created\n";
}

Chunk * Compressor1::compressFile(const char *file_name, int *num_chunks)
{
	int len;
	byte *buffer = (byte *)utils::read_text(file_name, &len);
	int extra = ((len % utils::CHUNK_SIZE) != 0);

	n_chunks = len / utils::CHUNK_SIZE + extra;
	int counter = 0;

	int numbytes = 0;
	for(int i = 0; i < n_chunks-1; i++) {
		unsigned long *size = (unsigned long*)malloc(sizeof(long));
		*size = utils::sizeCompressed;
		byte *dataCompressed = (byte*)malloc(sizeof(byte)*utils::sizeCompressed);
		compress2(dataCompressed, size, &buffer[counter], utils::CHUNK_SIZE, compressionRate);
		counter += utils::CHUNK_SIZE;

		listChunk[i].data = dataCompressed;
		listChunk[i].len = size;
		numbytes += *size;
	}

	// Compress last chunk
	int last_chunk = len-counter;
	unsigned long *size = (unsigned long*)malloc(sizeof(long));
	*size = utils::sizeCompressed;
	byte *dataCompressed = (byte*)malloc(utils::sizeCompressed);
	int z_result = compress2(dataCompressed, size, &buffer[counter], last_chunk, compressionRate);

	// Reallocate two bytes to save chunk size
	*size = (*size) + 2;
	byte *dataExtended = (byte*)realloc(dataCompressed, *size);
	if(dataExtended == NULL){
		COUT << "ERROR REALLOCATING";
		exit(0);
	}

	//Parse the last chunk size
	byte byte1 = last_chunk & 0x000000ff;
	byte byte2 = (last_chunk & 0x0000ff00) >> 8;
	dataExtended[(*size) - 2] = byte2;
	dataExtended[(*size) - 1] = byte1;

	listChunk[n_chunks-1].data = dataCompressed;
	listChunk[n_chunks-1].len = size;
	numbytes += *size;

	*num_chunks = n_chunks;

	COUT << "Size Original file: " << len << "\n";
	COUT << "Size compressed file: " << numbytes << "\n";

	if(z_result >= 0)
		return listChunk;
	else
		return NULL;
}

int Compressor1::startDecompress(const char *file_name)
{
	outputFile = fopen(file_name, "wb");
	return outputFile == NULL;
}

int Compressor1::decompressChunk(Chunk *input, int chunk_size)
{
	sizeDataUncompressed = chunk_size;
	int z_result = uncompress(dataUncompressed, &sizeDataUncompressed, input->data, *(input->len));

	utils::printPacket(input->data, *(input->len), 2);

	if(z_result >= 0) {
		int ret = fwrite(dataUncompressed, sizeof(char), sizeDataUncompressed, outputFile);
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

Compressor1::~Compressor1()
{
	for(int i = 0; i < n_chunks; i++) {
		free(listChunk[i].data);
		free(listChunk[i].len);
	}
}

