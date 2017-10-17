//Compressor.hpp

#ifndef COMPRESSOR_H_
#define COMPRESSOR_H_

/***************** Base Class Compressor *****************/

class Compressor {

protected:
	float compressionRate;
	int n_partitions;

public:
	Compressor(float compRate, int n_part);

	virtual void compress(const char *input, char *output) = 0;

	virtual void decompress(const char *input, char *output) = 0;

	virtual ~Compressor();
};

/***************** Derived Class Compressor1 *****************/

class Compressor1: public Compressor {

public:
	Compressor1(float compRate, int n_part);

	virtual void compress(const char *input, char *output) override;

	virtual void decompress(const char *input, char *output) override;

	virtual ~Compressor1();
};

#endif /* COMPRESSOR_H_ */
