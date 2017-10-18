//Encoder.cpp

#include <iostream>
#include "Encoder.hpp"

/***************** Base Class Compressor *****************/

Encoder::Encoder(int code_l, int redun):
code_length(code_l), redundancy(redun) {}

Encoder::~Encoder() {}

/***************** Derived Class Compressor1 *****************/

EncoderRS::EncoderRS(int code_l, int redun):
Encoder(code_l, redun)
{
	/* Finite Field Parameters */
    const std::size_t field_descriptor                =   8;
    const std::size_t generator_polynomial_index      = 120;
    const std::size_t generator_polynomial_root_count =  6;

    /* Reed Solomon Code Parameters */
    code_length = code_l;
    fec_length  = redun;
    data_length = code_length - fec_length;

    /* Instantiate Finite Field and Generator Polynomials */
    const schifra::galois::field field(field_descriptor,
                                  schifra::galois::primitive_polynomial_size06,
                                  schifra::galois::primitive_polynomial06);

    schifra::galois::field_polynomial generator_polynomial(field);

    if (
        !schifra::make_sequential_root_generator_polynomial(field,
                                                            generator_polynomial_index,
                                                            generator_polynomial_root_count,
                                                            generator_polynomial)
       )
    {
      std::cout << "Error - Failed to create sequential root generator!" << std::endl;
      return 1;
    }

    /* Instantiate Encoder and Decoder (Codec) */
    encoder<code_length,fec_length,data_length>(field, generator_polynomial);
    decoder<code_length,fec_length,data_length>(field, generator_polynomial_index);

	std::cout << "Reed Solomon encoder created..." << std::endl;
}

int EncoderRS::encode(const char *input, char *output)
{
	std::cout << "RSEncoding..." << std::endl;

	encoder.encode(input, encoder_block);
    for (int k=0; k < code_length; k++) {
        output[k] = encoder_block[k];
    }
        
	return 0;
}

int EncoderRS::decode(const char *input, char *output)
{
	int error = 0;
	std::string corrected = "";
	corrected.resize(data_length, 0x00);

	std::cout << "RSDecoding..." << std::endl;

    spacket.assign(packet, code_length);
    data = spacket.substr(0, data_length);
    fec = spacket.substr(data_length, fec_length);
    
    myblock blocked<code_length,fec_length>(data ,fec);

	if(!decoder.decode(blocked)) {
		std::cout << "Error - Critical decoding failure! "
				  << "Msg: " << blocked.error_as_string() << std::endl;
		error = 1;

	} else if(!schifra::is_block_equivelent(blocked, spacket)) {
		std::cout << "Error - Error correction failed! " << std::endl;
		error = 1;

	} else{
		blocked.data_to_string(corrected);
		output = (char *)corrected;
		std::cout << "Decoded Correctly" << std::endl;

	return error;
}

EncoderRS::~EncoderRS() {
	std::cout << "EncodeRS destroyed..." << std::endl;
}

