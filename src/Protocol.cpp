//Protocol.cpp

#include <iostream>
#include "Protocol.hpp"
#include "comms.hpp"
#include "utils.hpp"

#include <cstdio>
#include <cstdlib> // for the random

# define PAYLOAD 25
# define LEN_ACK 4

/***************** Base Class Protocol *****************/

Protocol::Protocol(Compressor *comp, Encoder *enc, Socket *sck):
compressor(comp),
encoder(enc),
socket(sck) {}

Protocol::~Protocol() {}


/***************** Derived Class StopWait *****************/

StopWait::StopWait(Compressor *comp, Encoder *enc, Socket *sck):
Protocol(comp, enc, sck)
{
	std::cout << "Stop and wait created" << std::endl;
}

int StopWait::receive_text()
{
	char *packet = (char *)malloc(sizeof(char) * (comms::_code_l));
	std::string corrected = "";
	corrected.resize(comms::_data_l, 0x00);

	FILE *outputFile = fopen("output.txt", "w+b");
	std::string spacket;
	std::string data;
	std::string fec;

	int counter = 0;
	int n;
	int decode_info;
	int size;
	
	while (true) {
		
		// Read packet (blocking)
		n = socket->read_blocking(packet, comms::_code_l);

        if (n == 0) {
			printf("\n Connection closed \n");
			break;	
		}
		
		printf("\n\n*** Received packet %i *** \n", counter);
		std::cout << packet << std::endl;
		//print_file(packet, code_length, 0);
		
        spacket.assign(packet, comms::_code_l);
        data = spacket.substr(0, comms::_data_l);
        //fec = spacket.substr(data_length, fec_length);
        
        decode_info = 0; //encoder->decode(data, corrected);
        corrected = data;

        if(decode_info == 1){
        	printf("\n\nPaquet incorrecte:  \n");

			char nack[4] = {'B', 'A','A','D'};
			socket->write_socket(nack, LEN_ACK);
        }else{
	        printf("\n\nCorrected packet:  \n");

			size = (unsigned char) corrected[PAYLOAD];
			std::cout << "\n[ " << corrected.substr(0, size) << " ]\n " << std::endl;
			std::cout << "Size: " << size << std::endl;

	        fwrite((char *)corrected.substr(0, size).c_str(), sizeof(char), size, outputFile);
	        counter++;

	        // Send ACK
	        char ack[4] = {'G','O','O','D'};
	        socket->write_socket(ack, LEN_ACK);

		}

	}

	if (outputFile != NULL)
		fclose(outputFile);
	else
		printf("Error writing the file");

	return 0;


	std::cout << "Receiving text..." << std::endl;
	return 0;
}

int StopWait::send_text(char *text) {

	std::cout << "Sending text..." << std::endl;
	int len;
	char *buffer = utils::read_text(text, &len);

	char *data = (char *)malloc(sizeof(char) * (PAYLOAD));
    char *packet = (char *)malloc(sizeof(char) * comms::_code_l);

    int i = 0; //position in the number of packets
    int extra = ((len % PAYLOAD) != 0); //extra we need for the last packet
    int pay_len; //actual lenght of the payload of the current packet

    /* ACK parameters */
    char *packet_ack = (char *)malloc(sizeof(char) * (LEN_ACK));
    int timeout = 2000;
    int n;

    std::string message; //variable we use to put the data we are going to send


    while (i < len / PAYLOAD + extra) {

    	//CREATING PACKET --> CREATE A FUNCTION THAT DOES THIS
        
        data = &buffer[i*PAYLOAD]; //put in data the corresponding part of the file
        if ( (i + 1) * PAYLOAD > len ) { //Last packet may be smaller
            pay_len = len - ( i * PAYLOAD ); //Anar amb cuidado de posar els padding bits
        } else {
            pay_len = PAYLOAD; //if last packet is not smaller, then it has size = PAYLOAD_LEN
        }
        
        message.assign(data, pay_len); //put the data we are going to send in a string variable
        
        message.resize(comms::_code_l, 0x00); //fill with 0x00 if the data is not long enough to form the packet
        message[PAYLOAD] = (unsigned char) pay_len; //Add information of the size of the payload


        //ENCODING PACKET
        encoder->encode((char *)message.c_str(), packet); //use the encoder to encode the information
        
        printf("\n\n *** Packet %i **** \n\n", i);
        std::cout << " Size: " << pay_len << std::endl;
        std::cout << message << std::endl;

        //SENDING PACKET

        /* Passar pel canal i enviar*/
        // El canal genera errors per bit i amb certa probabilitat fa drop del paquet
        //utils::bsc(packet, comms::_code_l, 0.0);
        //double w = (double)rand()/ (double) RAND_MAX;
        //if (w>0.5){
        	socket->write_socket(packet, comms::_code_l);
        //}
        

        //WAITING FOR ACK

        /* Wait for ACK */
        int ret;

        n = socket->read_non_blocking(packet_ack, LEN_ACK, timeout, &ret);

        if (ret == 0) {
            // Timeout: send again
            printf("\n Timeout expired, resending \n");

            continue;
        }
        
        if (n == 0) {
            printf("\n Connection closed \n");
            break;  
        }


        // Check ACK
        int c = comms::is_ack(packet_ack);
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

StopWait::~StopWait() {
	std::cout << "Stop and Wait destroyed..." << std::endl;

}

