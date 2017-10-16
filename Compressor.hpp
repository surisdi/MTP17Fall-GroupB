// Compressor.hpp

#ifndef COMPRESSOR_H
#define COMPRESSOR_H


/**********************************************************************/

// Base class
class Compressor {
	public:
		Compressor(float compRate){
			compressionRate = compRate;
		}	

		// pure virtual function
		virtual void compress(char* input, char* output) = 0;
		virtual void decompress(char*  input, char* output) = 0;

	protected:
		float compressionRate;
};


/*********************************************************************/

// Compressor 1
class Compressor1: public Compressor{
	public:
		Compressor1(float compRate);
		void compress(char* input, char* output) override;
		void decompress(char*  input, char* output) override;
};


#endif
