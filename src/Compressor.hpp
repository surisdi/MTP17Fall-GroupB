//Compressor.hpp

#ifndef COMPRESSOR_HPP
#define COMPRESSOR_HPP

#include "utils.hpp"
/***************** Base Class Compressor *****************/

class Compressor {

protected:
	int compressionRate;
	int n_chunks;
	FILE *outputFile;
	Chunk listChunk[utils::MAX_CHUNK];


public:
	// Compression ratio 0-9 (9 maximum compression)
	Compressor(int compRate);

	virtual int compressFile(char *input, Chunk *output, int *num_chunk) = 0;

	virtual int startDecompress(char *file_name) = 0;

	virtual int closeDecompress() = 0;

	virtual int decompressChunk(Chunk *input, int chunk_size, char *output) = 0;

	virtual ~Compressor();
};

/***************** Derived Class Compressor1 *****************/

class Compressor1: public Compressor {

public:
	Compressor1(int compRate);

	virtual int compressFile(char *input, Chunk *output, int *num_chunk) override;

	virtual int startDecompress(char *file_name) override;

	virtual int closeDecompress() override;

	virtual int decompressChunk(Chunk *input, int chunk_size, char *output) override;

	virtual ~Compressor1();
};

#endif /* COMPRESSOR_H_ */
