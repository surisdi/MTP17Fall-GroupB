#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "comms.h"
#include "utils.h"
#include "server.h"

#include "schifra/schifra_galois_field.hpp"
#include "schifra/schifra_galois_field_polynomial.hpp"
#include "schifra/schifra_sequential_root_generator_polynomial_creator.hpp"
#include "schifra/schifra_reed_solomon_encoder.hpp"
#include "schifra/schifra_reed_solomon_decoder.hpp"
#include "schifra/schifra_reed_solomon_block.hpp"
#include "schifra/schifra_error_processes.hpp"

#define TCP_PORT 5005
#define PAYLOAD 222


int main()
{

	/* Finite Field Parameters */
	const std::size_t field_descriptor                =   8;
	const std::size_t generator_polynomial_index      = 120;
	const std::size_t generator_polynomial_root_count =  32;

	/* Reed Solomon Code Parameters */
	const std::size_t code_length = 255;
	const std::size_t fec_length  =  32;
	const std::size_t data_length = code_length - fec_length;

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
    typedef schifra::reed_solomon::encoder<code_length,fec_length,data_length> encoder_t;
    typedef schifra::reed_solomon::decoder<code_length,fec_length,data_length> decoder_t;

    const encoder_t encoder(field, generator_polynomial);
    const decoder_t decoder(field, generator_polynomial_index);

    /* Instantiate RS Block For Codec */
	typedef schifra::reed_solomon::block<code_length,fec_length> myblock;

	char *packet = (char *)malloc(sizeof(char) * (code_length));
	int socket = initiateServer(TCP_PORT);

	std::string corrected = "";
	corrected.resize(data_length, 0x00);

	FILE *outputFile = fopen("output.txt", "w+b");
	std::string spacket;
	std::string data;
	std::string fec;

	int counter = 1;
	int n = 1;
	int size;
	
	while (true) {
		n = readFromSocket(packet, code_length, socket);
        
        if (n == 0) {
			break;	
		}
		
		printf("\n\n*** Received packet %i *** \n", counter);
		print_file(packet, code_length, 0);
		
        spacket.assign(packet, code_length);
        data = spacket.substr(0, data_length);
        fec = spacket.substr(data_length, fec_length);
        
        myblock blocked(data ,fec);
        
        bool correct = true;
		if(!decoder.decode(blocked)) {
			std::cout << "Error - Critical decoding failure! "
					  << "Msg: " << blocked.error_as_string() << std::endl;
			correct = false;
		} else if(!schifra::is_block_equivelent(blocked, spacket)) {
			std::cout << "Error - Error correction failed! " << std::endl;
			correct = false;
		}

		// Simulem tambe que a vegades no el rep, ni be ni malament: no fa res amb probabilitat p
		float p = 0.01;
		float w = (double)rand()/ (double) RAND_MAX;
		if(w > p){
			if(correct == false){
				// Send NACK
				//send_nack(socket)
			}
			else{
				// Send ACK
				//send_ack(socket)

		        printf("\n\n Corrected packet:  \n");
				size = (unsigned char) corrected[PAYLOAD];
				std::cout << " [ " << corrected.substr(0, size) << " ] " << std::endl;
				printf("\n\n Size: %u", size);
				
		        fwrite((char *)corrected.substr(0, size).c_str(), sizeof(char), size, outputFile);
		        counter++;

			}
		}
	}

	if (outputFile != NULL)
		fclose(outputFile);
	else
		printf("Error writing the file");

	return 0;

}
