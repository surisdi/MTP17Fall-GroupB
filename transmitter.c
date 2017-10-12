#include <cstddef>
#include <iostream>
#include <string>
#include <unistd.h>

#include <RF24/RF24.h>
#include <unistd.h>
#include <sstream>


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

#define TCP_PORT 5005
#define PAYLOAD 25

#define CHANNEL 97


// GPIO025 (Pin 22) as CE, and CE0 as CS
// SPI speed can be added as well (BCM2835_SPI_SPEED_8MHz)
RF24 radio(25, 0); 

// Addresses for the pipes, we need one for writing and one for reading
const uint8_t addresses[][6] = {"1Node", "2Node"};

int main(int argc, char* argv[])
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
	
	// Transmitter will transmit first then read, that's why we first stop listening
	radio.stopListening();
	
	
	
	
	
	
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

    std::string message;
    

	/* Instantiate RS Block For Codec */
	schifra::reed_solomon::block<code_length,fec_length> block;
	
	/* Start socket */
	//int sockfd = initiateSocket ((char *)"localhost", TCP_PORT);

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
		
        message.resize(code_length, 0x00);
        message[PAYLOAD] = (unsigned char) pay_len; //Add information of the size of the payload
        encoder.encode(message, block);
        
        printf("\n\n *** Packet %i **** \n\n", i);

        for (int k=0; k < code_length; k++) {
            printf(" %u ", block[k]);
            packet[k] = block[k];
        }
        
        //bool noisy_channel = true;
        //send(packet, code_length, sockfd); //noisy_channel);
        bool ok = radio.write( packet, code_length, 1 ); //Mirar el 1 del final en teoria es perque no esperi ack ni res
        
		i++;
    }

   return 0;
}
