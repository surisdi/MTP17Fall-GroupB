//Encoder.hpp

#ifndef ENCODER_H_
#define ENCODER_H_

#include <string>
#include <unistd.h>
#include <cstddef>

// #include "schifra/schifra_galois_field.hpp"
// #include "schifra/schifra_galois_field_polynomial.hpp"
// #include "schifra/schifra_sequential_root_generator_polynomial_creator.hpp"
// #include "schifra/schifra_reed_solomon_encoder.hpp"
// #include "schifra/schifra_reed_solomon_decoder.hpp"
// #include "schifra/schifra_reed_solomon_block.hpp"
// #include "schifra/schifra_error_processes.hpp"

#include "comms.hpp"

/***************** Base Class Encoder *****************/

class Encoder {

public:
	Encoder();

	// TODO: CANVIAR ELS NOMS DE LES FUNCIONS PK NO COINCIDEIXIN AMB EL "encode" de la llibreria

	virtual int encode(std::string input, char *output) = 0;

	virtual int encode_ack(char *input, char *output) = 0;

	virtual int decode(std::string input, std::string  output) = 0;

	virtual int decode_ack(char *input, char *output) = 0;

	virtual ~Encoder();
};


/***************** Derived Class EncoderRS *****************/
/*
class EncoderRS: public Encoder {
public:
	EncoderRS();

	virtual int encode(std::string input, char *output) override;

	virtual int encode_ack(const char *input, char *output) override;

	virtual int decode(const char *input, char *output) override;

	virtual int decode_ack(const char *input, char *output) override;

	virtual ~EncoderRS();

private: 

	// Finite Field Parameters 
	const std::size_t field_descriptor = 8;
	const std::size_t generator_polynomial_index = 120;
	std::size_t generator_polynomial_root_count = 6;
	std::size_t ack_generator_polynomial_root_count = 6;

	// Reed Solomon Code Parameters 
	struct s_code_param {
		std::size_t code_length;
		std::size_t fec_length;
		std::size_t data_length;
	};

	s_code_param s_code;
	
	//static const s_code_param s_code = {comms::code_length,
	//									comms::fec_length,
	//									comms::data_length};
	//
	//static const s_code_param s_ack = {comms::ack_code_length,
	//									comms::ack_fec_length,
	//								comms::ack_data_length};


 	// //    Instantiate Finite Field and Generator Polynomials 
	// schifra::galois::field *field;

	// schifra::galois::field_polynomial *generator_polynomial;

    // Instantiate Encoder and Decoder (Codec) 
	typedef schifra::reed_solomon::shortened_encoder<32, 6, 26> encoder_t;
	typedef schifra::reed_solomon::shortened_decoder<32, 6, 26> decoder_t;

	encoder_t *encoder;
	decoder_t *decoder;

	// Instantiate RS Block For Codec 
	schifra::reed_solomon::block<32, 6> block;


	std::string spacket;
	std::string data;
	std::string fec;
	std::string corrected;
};*/

/***************** Derived Class Encoder Dummy *****************/

class EncoderDummy: public Encoder {
public:
	EncoderDummy();

	virtual int encode(std::string input, char *output) override;

	virtual int encode_ack(char *input, char *output) override;

	virtual int decode(std::string  input, std::string  output) override;

	virtual int decode_ack(char *input, char *output) override;

	virtual ~EncoderDummy();

};

#endif /* ENCODER_H_ */

