// Encoder.hpp

#ifndef ENCODER_H
#define ENCODER_H

/**********************************************************************/

// Base class
class Encoder {
	public:
		// pure virtual function
		virtual void encode(char* input, char* output) = 0;
		virtual void decode(char* input, char* output) = 0;
}


/*********************************************************************/

// Reed Solomon Encoder
class EncoderReedSolomon: public Encoder{

	EncoderReedSolomon();
	void encode(char* input, char* output);
	void decode(char* input, char* output);
}

/************************************************************************/
