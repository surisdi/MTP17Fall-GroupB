//Encoder.hpp

#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <string>
#include <unistd.h>
#include <cstddef>

#include "schifra/schifra_galois_field.hpp"
#include "schifra/schifra_galois_field_polynomial.hpp"
#include "schifra/schifra_sequential_root_generator_polynomial_creator.hpp"
#include "schifra/schifra_reed_solomon_encoder.hpp"
#include "schifra/schifra_reed_solomon_decoder.hpp"
#include "schifra/schifra_reed_solomon_block.hpp"
#include "schifra/schifra_error_processes.hpp"

#include "comms.hpp"

/***************** Base Class Encoder *****************/


class Encoder {

public:
	Encoder(){}

	virtual int encode(const char *input, char *output) = 0;

	virtual int decode(const char *input, char *output) = 0;

	virtual ~Encoder(){}
};

/***************** Derived Class EncoderRS *****************/

template <std::size_t code_length, std::size_t fec_length, std::size_t data_length = code_length - fec_length>
class EncoderRS: public Encoder {
public:
	inline EncoderRS() {
		field = new schifra::galois::field(field_descriptor,
				schifra::galois::primitive_polynomial_size06,
				schifra::galois::primitive_polynomial06);

		generator_polynomial = new schifra::galois::field_polynomial(*field);

		if (
		        !schifra::make_sequential_root_generator_polynomial(*field,
		                                                            generator_polynomial_index,
		                                                            generator_polynomial_root_count,
		                                                            *generator_polynomial)
		       )
		{
		  std::cout << "Error - Failed to create sequential root generator!" << std::endl;
		}

		encoder = new encoder_t(*field, *generator_polynomial);
		decoder = new decoder_t(*field, generator_polynomial_index);

		corrected.resize(data_length, 0x00);
	}

	inline int encode(const char *input, char *output) {
		std::cout << "RSEncoding..." << std::endl;

		spacket.assign(input, code_length);
		encoder->encode(input, block);
		for (int k=0; k < code_length; k++) {
			output[k] = block[k];
		}

		return 0;
	}

	inline int decode(const char *input, char *output) {
		int error = 0;

		spacket.assign(input, code_length);
		data = spacket.substr(0, data_length);
		fec = spacket.substr(data_length, fec_length);

		block = schifra::reed_solomon::block<code_length,fec_length>(data, fec);

		if(!decoder->decode(block)) {
			std::cout << "Error - Critical decoding failure! "
					  << "Msg: " << block.error_as_string() << std::endl;
			error = 1;

		} else{
			block.data_to_string(corrected);
			output = (char *)corrected.c_str();
		}

		return error;
	}

	inline ~EncoderRS() {
		std::cout << "EncodeRS destroyed..." << std::endl;
	}

private:
	/* Finite Field Parameters */
	const std::size_t field_descriptor = 8;
	const std::size_t generator_polynomial_index = 120;
	const std::size_t generator_polynomial_root_count = fec_length;

    /* Instantiate Finite Field and Generator Polynomials */
	schifra::galois::field *field;
	schifra::galois::field_polynomial *generator_polynomial;

    /* Instantiate Encoder and Decoder (Codec) */
	typedef schifra::reed_solomon::shortened_encoder<code_length, fec_length, data_length> encoder_t;
	typedef schifra::reed_solomon::shortened_decoder<code_length, fec_length, data_length> decoder_t;

	encoder_t *encoder;
	decoder_t *decoder;

	/* Instantiate RS Block For Codec */
	schifra::reed_solomon::block<code_length, fec_length,data_length> block;

	std::string spacket;
	std::string data;
	std::string fec;
	std::string corrected;
};

#endif /* ENCODER_H_ */
