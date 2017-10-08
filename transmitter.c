#include <cstddef>
#include <iostream>
#include <string>

#include "comms.h"
#include "utils.h"
#include "client.h"

#include "schifra/schifra_galois_field.hpp"
#include "schifra/schifra_galois_field_polynomial.hpp"
#include "schifra/schifra_sequential_root_generator_polynomial_creator.hpp"
#include "schifra/schifra_reed_solomon_encoder.hpp"
#include "schifra/schifra_reed_solomon_decoder.hpp"
#include "schifra/schifra_reed_solomon_block.hpp"
#include "schifra/schifra_error_processes.hpp"

#define PAYLOAD 222
#define TCP_PORT 5005


int main(int argc, char* argv[])
{
	/* Deal with arguments */
	
	const char* file_name = "";
	if(argc < 2){
		file_name = "file.txt";
	}
	else{
		file_name = argv[1];
	}

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

	/* Read file */
    /*std::ifstream t("file.txt");
    std::string str;

    t.seekg(0, std::ios::end);
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());*/
    
    /* Read file byte array style */
    int len;
    char *buffer = readFileBytes(file_name, &len);
    char *data = (char *)malloc(sizeof(char) * (PAYLOAD));
    char *packet = (char *)malloc(sizeof(char) * code_length);
        
    std::string str(buffer);
    std::string message;
    

	/* Instantiate RS Block For Codec */
	schifra::reed_solomon::block<code_length,fec_length> block;
	
	/* Start socket */
	int sockfd = initiateSocket ((char *)"localhost", TCP_PORT);

	//len = PAYLOAD*2 + 3;
	int i = 0;
    int extra = ((len % PAYLOAD) != 0);
    int pay_len;
    while (i < len / PAYLOAD + extra) {
		
        data = &buffer[i*PAYLOAD];
        if ( (i + 1) * PAYLOAD > len ) { //Last packet may be smaller
            pay_len = len - ( i * PAYLOAD ); //Anar amb cuidado de posar els padding bits
        } else {
            pay_len = PAYLOAD;
        }
        
        message.assign(data, pay_len);

		print_file(data, pay_len, 0);
		
        message.resize(code_length, 0x00);
        message[PAYLOAD] = (char) pay_len; //Add information of the size of the payload
        encoder.encode(message, block);
        
        printf("\n\n *** Packet %i **** \n\n", i);
        
        //printf("\n 4 \n");
        for (int k=0; k < code_length; k++) {
            printf(" %u ", block[k]);
            packet[k] = block[k];
        }
        
        bool noisy_channel = true;
        send(packet, code_length, sockfd, noisy_channel);

        // Stop and wait

        int is_ack;
        char *packet[L_ACK]
        // Aqui hauriem de posar un timer, i que el receive() no sigui bloquejant
        // Mirar web http://developerweb.net/viewtopic.php?id=3196
        n = receive(packet, L_ACK, socket);
        is_ack = isAck(packet)

        if(is_ack == 0){ // Missatge correcte
        	i++;
        }
    }

   return 0;
}
