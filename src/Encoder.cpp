//Encoder.cpp

#include <iostream>
#include "Encoder.hpp"

/***************** Base Class Compressor *****************/

Encoder::Encoder(){}

Encoder::~Encoder() {}

/***************** Derived Class Compressor1 *****************/

EncoderRS::EncoderRS()
{
	field = schifra::galois::field(field_descriptor,
			schifra::galois::primitive_polynomial_size06,
			schifra::galois::primitive_polynomial06);

	generator_polynomial = schifra::galois::field_polynomial(field);
	ack_generator_polynomial = schifra::galois::field_polynomial(field);
    if (
        !schifra::make_sequential_root_generator_polynomial(field,
                                                            generator_polynomial_index,
                                                            generator_polynomial_root_count,
                                                            generator_polynomial)
       )
    {
      std::cout << "Error - Failed to create sequential root generator!" << std::endl;
    }

    if (
		!schifra::make_sequential_root_generator_polynomial(field,
															generator_polynomial_index,
															ack_generator_polynomial_root_count,
															ack_generator_polynomial)
	   )
	{
	  std::cout << "Error - Failed to create sequential root generator!" << std::endl;
	}

    encoder = encoder_t(field, generator_polynomial);
    decoder = decoder_t(field, generator_polynomial_index);

    ack_encoder = ack_encoder_t(field, ack_generator_polynomial);
    ack_decoder = ack_decoder_t(field, generator_polynomial_index);

    corrected.resize(s_code.data_length, 0x00);
    ack_corrected.resize(s_ack.data_length, 0x00);
}

int EncoderRS::encode(const char *input, char *output)
{
	std::cout << "RSEncoding..." << std::endl;

	spacket.assign(input, s_code.code_length);
	encoder.encode(input, block);
	for (int k=0; k < s_code.code_length; k++) {
		output[k] = block[k];
	}
        
	return 0;
}

int EncoderRS::encode_ack(const char *input, char *output)
{
	spacket.assign(input, s_ack.code_length);
	ack_encoder.encode(input, ack_block);
	for (int k=0; k < s_ack.code_length; k++) {
		output[k] = ack_block[k];
	}

	return 0;
}

int EncoderRS::decode(const char *input, char *output)
{
	int error = 0;

	spacket.assign(input, s_code.code_length);
	data = spacket.substr(0, s_code.data_length);
	fec = spacket.substr(s_code.data_length, s_code.fec_length);

    block = schifra::reed_solomon::block<s_code.code_length,s_code.fec_length>(data, fec);

	if(!decoder.decode(block)) {
		std::cout << "Error - Critical decoding failure! "
				  << "Msg: " << block.error_as_string() << std::endl;
		error = 1;

	} else{
		block.data_to_string(corrected);
		output = (char *)corrected;
	}

	return error;
}

int EncoderRS::decode_ack(const char *input, char *output)
{
	int error = 0;

	spacket.assign(input, s_ack.code_length);
	data = spacket.substr(0, s_ack.data_length);
	fec = spacket.substr(s_ack.data_length, s_ack.fec_length);

    ack_block = schifra::reed_solomon::block<s_ack.code_length,s_ack.fec_length>(data, fec);

	if(!ack_decoder.decode(ack_block)) {
		std::cout << "Error - Critical decoding failure! "
				  << "Msg: " << ack_block.error_as_string() << std::endl;
		error = 1;

	} else{
		ack_block.data_to_string(ack_corrected);
		output = (char *)ack_corrected;
	}

	return error;
}


EncoderRS::~EncoderRS() {
	std::cout << "EncodeRS destroyed..." << std::endl;
}

