#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

#include <RF24/RF24.h>
#include <unistd.h>
#include <sstream>


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
#define PAYLOAD 25

#define CHANNEL 97

// GPIO025 (Pin 22) as CE, and CE0 as CS
// SPI speed can be added as well (BCM2835_SPI_SPEED_8MHz)
RF24 radio(25, 0); 

// Addresses for the pipes, we need one for writing and one for reading
const uint8_t addresses[][6] = {"1Node", "2Node"};

int main()
{
	/* Define radio parameters */
	
	radio.begin();
	radio.setChannel(CHANNEL); //2400 + n(MHz)
	// Power Amplifier Level: MIN(-18dBm), LOW (-12dBm), HIGH (-6dBm), MAX (0 dBm)
	radio.setPALevel(RF24_PA_LOW);
	// Data rate: 250kbps, 1MBPS, 2MBPS
	radio.setDataRate(RF24_2MBPS);
	radio.setAutoAck(0);
	radio.disableCRC();
	
	printf("\n ** Radio configuration ** \n");
	radio.printDetails();
	
	radio.openWritingPipe(addresses[0]); // Open Pipes
	// Up to 6 pipes can be open for reading, open pipe number 1 and give addresss
	// Suposu que es per llegir mes rapid...
	radio.openReadingPipe(1, addresses[1]);  
	
	// Now receiver will only receive
	radio.startListening();
	

	/* Finite Field Parameters */
	const std::size_t field_descriptor                =   8;
	const std::size_t generator_polynomial_index      = 120;
	const std::size_t generator_polynomial_root_count =  6;

	/* Reed Solomon Code Parameters */
	const std::size_t code_length = 32;
	const std::size_t fec_length  =  6;
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
    typedef schifra::reed_solomon::shortened_encoder<code_length,fec_length,data_length> encoder_t;
    typedef schifra::reed_solomon::shortened_decoder<code_length,fec_length,data_length> decoder_t;

    const encoder_t encoder(field, generator_polynomial);
    const decoder_t decoder(field, generator_polynomial_index);

    /* Instantiate RS Block For Codec */
	typedef schifra::reed_solomon::block<code_length,fec_length> myblock;

	char *packet = (char *)malloc(sizeof(char) * (code_length));
	//int socket = initiateServer(TCP_PORT);

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
		
		while( ! radio.available() ) {}
		
		//n = readFromSocket(packet, code_length, socket);
		radio.read(packet, code_length);
        
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
		}

		printf("BOOL: %u", blocked.data_to_string(corrected));
		printf("\n\n Corrected packet:  \n");

		size = (unsigned char) corrected[PAYLOAD];
		std::cout << " [ " << corrected.substr(0, size) << " ] " << std::endl;
		printf("\n\n Size: %u", size);

		fwrite((char *)corrected.substr(0, size).c_str(), sizeof(char), size, outputFile);
		counter++;
	}

	if (outputFile != NULL)
		fclose(outputFile);
	else
		printf("Error writing the file");

	return 0;

}
