//Compressor.hpp

#ifndef COMPRESSOR_HPP
#define COMPRESSOR_HPP

#include "utils.hpp"
/***************** Base Class Compressor *****************/

class Compressor {

protected:
	int compressionRate;
	FILE *outputFile;
	byte dataUncompressed[utils::CHUNK_SIZE];
	unsigned long sizeDataUncompressed;

public:
	Chunk listChunk[utils::MAX_CHUNK];
	int n_chunks;




public:
	// Compression ratio 0-9 (9 maximum compression)
	Compressor(int compRate);

	virtual Chunk * compressFile(const char *input, int *num_chunks) = 0;

	virtual int startDecompress(const char *file_name) = 0;

	virtual int closeDecompress() = 0;

	virtual int decompressChunk(Chunk *input, int chunk_size) = 0;

	virtual ~Compressor();
};

/***************** Derived Class Compressor1 *****************/

class Compressor1: public Compressor {

public:
	Compressor1(int compRate);

	virtual Chunk * compressFile(const char *input, int *num_chunks) override;

	virtual int startDecompress(const char *file_name) override;

	virtual int closeDecompress() override;

	virtual int decompressChunk(Chunk *input, int chunk_size) override;

	virtual ~Compressor1();
};

#endif /* COMPRESSOR_H_ */
