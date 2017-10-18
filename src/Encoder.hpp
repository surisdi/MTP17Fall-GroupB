//Encoder.hpp

#ifndef ENCODER_H_
#define ENCODER_H_

#include "schifra/schifra_galois_field.hpp"
#include "schifra/schifra_galois_field_polynomial.hpp"
#include "schifra/schifra_sequential_root_generator_polynomial_creator.hpp"
#include "schifra/schifra_reed_solomon_encoder.hpp"
#include "schifra/schifra_reed_solomon_decoder.hpp"
#include "schifra/schifra_reed_solomon_block.hpp"
#include "schifra/schifra_error_processes.hpp"

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

private:
	const std::size_t code_length;
    const std::size_t fec_length;
    const std::size_t data_length;

    template<std::size_t cl,std::size_t fl,std::size_t dl>
	using encoder_t = schifra::reed_solomon::shortened_encoder<cl,fl,dl>;
	template<std::size_t cl,std::size_t fl,std::size_t dl>
	using decoder_t = schifra::reed_solomon::shortened_decoder<cl,fl,dl>;

	template<std::size_t cl,std::size_t fl,std::size_t dl>
	encoder_t<std::size_t,std::size_t,std::size_t> encoder;
    //const decoder_t decoder;

    /* Instantiate RS Block For Codec */
    schifra::reed_solomon::block<std::size_t,std::size_t> encoder_block;
    template<std::size_t cl,std::size_t fl>
   	using decoder_block = schifra::reed_solomon::block<cl,fl>;

	std::string spacket;
	std::string data;
	std::string fec;
};

#endif /* ENCODER_H_ */
