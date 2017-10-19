//Encoder.hpp

#ifndef ENCODER_H_
#define ENCODER_H_

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
	Encoder();

	virtual int encode(const char *input, char *output) = 0;

	virtual int encode_ack(const char *input, char *output) = 0;

	virtual int decode(const char *input, char *output) = 0;

	virtual int decode_ack(const char *input, char *output) = 0;

	virtual ~Encoder();
};


/***************** Derived Class EncoderRS *****************/

class EncoderRS: public Encoder {
public:
	EncoderRS();

	virtual int encode(const char *input, char *output) override;

	virtual int encode_ack(const char *input, char *output) override;

	virtual int decode(const char *input, char *output) override;

	virtual int decode_ack(const char *input, char *output) override;

	virtual ~EncoderRS();

private:

	/* Finite Field Parameters */
	const std::size_t field_descriptor = 8;
	const std::size_t generator_polynomial_index = 120;
	std::size_t generator_polynomial_root_count = comms::fec_length;
	std::size_t ack_generator_polynomial_root_count = comms::ack_fec_length;

	/* Reed Solomon Code Parameters */
	struct s_code_param {
		std::size_t code_length;
		std::size_t fec_length;
		std::size_t data_length;
	};

	const s_code_param s_code = {comms::code_length,
									comms::fec_length,
									comms::data_length};

	const s_code_param s_ack = {comms::ack_code_length,
									comms::ack_fec_length,
									comms::ack_data_length};

    /* Instantiate Finite Field and Generator Polynomials */
	schifra::galois::field field;
	schifra::galois::field_polynomial generator_polynomial;

	schifra::galois::field_polynomial ack_generator_polynomial;

    /* Instantiate Encoder and Decoder (Codec) */
	typedef schifra::reed_solomon::shortened_encoder<s_code.code_length,
														s_code.fec_length,s_code.data_length> encoder_t;
	typedef schifra::reed_solomon::shortened_decoder<s_code.code_length,
														s_code.fec_length,s_code.data_length> decoder_t;

	encoder_t encoder;
	decoder_t decoder;

    /* Instantiate ACK Encoder and Decoder (Codec) */
	typedef schifra::reed_solomon::shortened_encoder<s_ack.code_length,
													s_ack.fec_length,s_ack.data_length> ack_encoder_t;
	typedef schifra::reed_solomon::shortened_decoder<s_ack.code_length,
													s_ack.fec_length,s_ack.data_length> ack_decoder_t;

	ack_encoder_t ack_encoder;
	ack_decoder_t ack_decoder;

	/* Instantiate RS Block For Codec */
	schifra::reed_solomon::block<s_code.code_length,s_code.fec_length> block;

	/* Instantiate RS Block For ACK Codec */
	schifra::reed_solomon::block<s_ack.code_length,s_ack.fec_length> ack_block;

	std::string spacket;
	std::string data;
	std::string fec;
	std::string corrected;
	std::string ack_corrected;
};

#endif /* ENCODER_H_ */
