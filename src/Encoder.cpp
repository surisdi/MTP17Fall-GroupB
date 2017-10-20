//Encoder.cpp

#include <iostream>
#include "Encoder.hpp"

#define code_length 32
#define data_length 26

/***************** Base Class Compressor *****************/

Encoder::Encoder(){}

Encoder::~Encoder() {}

// /***************** Derived Class EncoderRS *****************/

// EncoderRS::EncoderRS()
// {
// 	s_code = {32, 6, 26};

// 	schifra::galois::field field (field_descriptor,
// 			schifra::galois::primitive_polynomial_size06,
// 			schifra::galois::primitive_polynomial06);

// 	schifra::galois::field_polynomial generator_polynomial(field);

//     if (
//         !schifra::make_sequential_root_generator_polynomial(field,
//                                                             generator_polynomial_index,
//                                                             generator_polynomial_root_count,
//                                                             generator_polynomial)
//        )
//     {
//       std::cout << "Error - Failed to create sequential root generator!" << std::endl;
//     }

//     encoder = new encoder_t(field, generator_polynomial);
//     decoder = new decoder_t(field, generator_polynomial_index);


//     corrected.resize(s_code.data_length, 0x00);
// }


// int EncoderRS::encode(std::string input, char *output)
// {
// 	std::cout << "RSEncoding..." << std::endl;

// 	//spacket.assign(input, s_code.code_length);
// 	encoder->encode(input, block);
// 	for (int k=0; k < s_code.code_length; k++) {
// 		output[k] = block[k];
// 	}
        
// 	return 0;
// }

// int EncoderRS::encode_ack(const char *input, char *output)
// {
// 	// spacket.assign(input, s_ack.code_length);
// 	// ack_encoder.encode(input, ack_block);
// 	// for (int k=0; k < s_ack.code_length; k++) {
// 	// 	output[k] = ack_block[k];
// 	// }

// 	return 0;
// }

// int EncoderRS::decode(const char *input, char *output)
// {
// 	int error = 0;

// 	spacket.assign(input, s_code.code_length);
// 	data = spacket.substr(0, s_code.data_length);
// 	fec = spacket.substr(s_code.data_length, s_code.fec_length);

//     block = schifra::reed_solomon::block<32, 6>(data, fec);

// 	if(!decoder->decode(block)) {
// 		std::cout << "Error - Critical decoding failure! "
// 				  << "Msg: " << block.error_as_string() << std::endl;
// 		error = 1;

// 	} else{
// 		block.data_to_string(corrected);
// 		output = (char *)corrected.c_str();
// 	}

// 	return error;
// }

// int EncoderRS::decode_ack(const char *input, char *output)
// {
// 	int error = 0;

// 	// spacket.assign(input, s_ack.code_length);
// 	// data = spacket.substr(0, s_ack.data_length);
// 	// fec = spacket.substr(s_ack.data_length, s_ack.fec_length);

//  //    ack_block = schifra::reed_solomon::block<s_ack.code_length,s_ack.fec_length>(data, fec);

// 	// if(!ack_decoder.decode(ack_block)) {
// 	// 	std::cout << "Error - Critical decoding failure! "
// 	// 			  << "Msg: " << ack_block.error_as_string() << std::endl;
// 	// 	error = 1;

// 	// } else{
// 	// 	ack_block.data_to_string(ack_corrected);
// 	// 	output = (char *)ack_corrected;
// 	// }

// 	return error;
// }


// EncoderRS::~EncoderRS() {
// 	std::cout << "EncodeRS destroyed..." << std::endl;
// }



/***************** Derived Class Encoder Dummy *****************/

EncoderDummy::EncoderDummy()
{}

int EncoderDummy::encode(std::string input, char *output)
{
	std::cout << "Encoding Dummy..." << std::endl;

	for (int k=0; k < code_length; k++) {
		output[k] = input[k];
	}
        
	return 0;
}

int EncoderDummy::encode_ack(char *input, char *output)
{
	std::cout << "Encoding ack..." << std::endl;
	return 0;
}

int EncoderDummy::decode(std::string input, std::string output)
{

	std::cout << "Decoding..." << std::endl;
	for (int k=0; k < data_length; k++) {
		output[k] = input[k];
	}
	return 0;
}

int EncoderDummy::decode_ack(char *input, char *output)
{

	std::cout << "ACK decoded..." << std::endl;
	return 0;
}


EncoderDummy::~EncoderDummy() {
	std::cout << "EncodeRS destroyed..." << std::endl;
}


