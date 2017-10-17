//Encoder.hpp

#ifndef ENCODER_H_
#define ENCODER_H_

/***************** Base Class Encoder *****************/

class Encoder {

protected:
	int code_length;
	int redundancy;

public:
	Encoder(int code_l, int redun);

	virtual int encode(const char *input, char *output) = 0;

	virtual int decode(const char *input, char *output) = 0;

	virtual ~Encoder();
};


/***************** Derived Class EncoderRS *****************/

class EncoderRS: public Encoder {
public:
	EncoderRS(int code_l, int redun);

	virtual int encode(const char *input, char *output) override;

	virtual int decode(const char *input, char *output) override;

	virtual ~EncoderRS();
};

#endif /* ENCODER_H_ */
