#include <cstddef>
#include <iostream>
#include <string>
#include <unistd.h>

#include "comms.h"
#include "utils.h"
#include "client.h"
//#include "poll.h"

#include "schifra/schifra_galois_field.hpp"
#include "schifra/schifra_galois_field_polynomial.hpp"
#include "schifra/schifra_sequential_root_generator_polynomial_creator.hpp"
#include "schifra/schifra_reed_solomon_encoder.hpp"
#include "schifra/schifra_reed_solomon_decoder.hpp"
#include "schifra/schifra_reed_solomon_block.hpp"
#include "schifra/schifra_error_processes.hpp"

#define TCP_PORT 2222
#define PAYLOAD 25
#define LEN_ACK 4

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
    std::cout << "Input file name: " << file_name << std::endl;

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

    /* Read file byte array style */
    int len;
    char *buffer = readFileBytes(file_name, &len);
    char *data = (char *)malloc(sizeof(char) * (PAYLOAD));
    char *packet = (char *)malloc(sizeof(char) * code_length);
        
    std::string message;
    

    /* Instantiate RS Block For Codec */
    schifra::reed_solomon::block<code_length,fec_length> block;
    
    /* Start socket */
    int socket = initiateSocket ((char *)"localhost", TCP_PORT);
    int ret;

    //len = PAYLOAD*2 + 3;
    int i = 0;
    int extra = ((len % PAYLOAD) != 0);
    int pay_len;

    /* ACK parameters */
    char *packet_ack = (char *)malloc(sizeof(char) * (LEN_ACK));
    int timeout = 2000;
    int n;

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
        std::cout << " Size: " << pay_len << std::endl;
        
        for (int k=0; k < code_length; k++) {
            // printf(" %u ", block[k]);
            packet[k] = block[k];
        }
        
        /* Passar pel canal i enviar*/
        // El canal genera errors per bit i amb certa probabilitat fa drop del paquet
        send_channel(packet, code_length, socket);
        //usleep(2000000);

        /* Wait for ACK */
        n = read_with_timeout(socket, timeout, packet_ack, LEN_ACK, &ret);

        if (ret == 0) {
            // Timeout: send again
            continue;
        }
        
        if (n == 0) {
            printf("\n Connection closed \n");
            break;  
        }

        // Check ACK
        int c = isAck(packet_ack);
        switch(c){
            case 1: // is ACK
                printf("ACK received\n");
                i++;
                break;
            case -1:
                printf("NACK received\n");
                break;
            default:
                printf("Garbage\n");
        }        
    }

    printf("Finished transmitting!\n");

    return 0;
}
