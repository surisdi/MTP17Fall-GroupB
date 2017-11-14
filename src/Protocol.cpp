
//Protocol.cpp

#include <iostream>
#include "Protocol.hpp"
#include "utils.hpp"

#include <cstdio>
#include <cstdlib> // for the random
#include <unistd.h>
#include <bitset>

/***************** Base Class Protocol *****************/

Protocol::Protocol(Compressor *comp, Encoder *enc, Socket *sck):
compressor(comp),
encoder(enc),
socket(sck) {}

int Protocol::isAck(const char* r_ack) {
        //std::bitset<sizeof(char)> bs(r_ack);
    //Forgive me god, for i have sinned
	int n_ones = 0;
	int n_zeros = 0;

	for (int k = 0; k < 8; k++) {
		if((*r_ack) & (0x01 << k) != 0) {
			n_ones++;
		}
	}

	if (n_ones > 5) {
		return 1;
	}else if(n_ones < 3) {
		return 0;
	} else {
		return -1;
	}
}

int Protocol::createPacket(char *){}

Protocol::~Protocol() {}


/***************** Derived Class StopWait *****************/

StopWait::StopWait(Compressor *comp, Encoder *enc, Socket *sck):
Protocol(comp, enc, sck)
{
    std::cout << "Stop and wait created" << std::endl;
}

int StopWait::receive_text() {
    
    std::cout << "Receiving text..." << std::endl;
    
    char packet[utils::CODE_L];
    char corrected[utils::CODE_L];
    
    FILE *outputFile = fopen("output.txt", "w+b");
    
    int counter = 0;
    bool error;
    bool result;
    int decode_info;
    unsigned int size;
    bool last_packet = false;
    
    while (!last_packet) {
        result = socket->read_blocking(packet, utils::CODE_L);
        
        if (!result) {
            printf("Connection closed \n");
            break;
        }
        
        printf("*** Received packet %i *** \n", counter);
        utils::printPacket(packet, utils::CODE_L, 1);
        
        error = encoder->decode(packet, corrected);
        
        if(error){
            printf("Paquet incorrecte:  \n");
            socket->write_socket(&utils::nack, 1, 1);
        }else{
            printf("Corrected packet:  \n");

            utils::printPacket(corrected, utils::PAYLOAD_L, 1);
            size = (unsigned int) corrected[utils::PAYLOAD_L];

            if (size > utils::PAYLOAD_L){
                size = size - 100;
                last_packet = true;
            }
            std::cout << "Size: " << size << std::endl;

            fwrite(corrected, sizeof(char), size, outputFile);
            counter++;

            socket->write_socket(&utils::ack, 1, 1);
        }
        
    }
    
    if (outputFile != NULL) {
        fclose(outputFile);
        return 1;
    } else {
        printf("Error writing the file");
        return 0;
    }
    
}

int StopWait::send_text(char *text) {
    
    std::cout << "Sending text..." << std::endl;
    
    int len;
    int size;
    char *buffer = utils::read_text(text, &len);
    
    //char data[utils::PAYLOAD_L];
    char packet[utils::CODE_L];
    char packet_ack[utils::LEN_ACK];
    char message[utils::CODE_L];
    
    memset(message, 0x00, utils::CODE_L);
    
    int i = 0; //position in the number of packets
    int extra = ((len % utils::PAYLOAD_L) != 0); //extra we need for the last packet
    int pay_len; //actual lenght of the payload of the current packet
    
    /* ACK parameters */
    int timeout = 50;
    int n;

    int flagOut = 0;
    while (i < len / utils::PAYLOAD_L + extra) {
        
        if ( (i + 1) * utils::PAYLOAD_L > len ) {
            pay_len = len - ( i * utils::PAYLOAD_L );
        } else {
            pay_len = utils::PAYLOAD_L;
        }

	if(i*utils::PAYLOAD_L + pay_len  == len){
		flagOut = 1;
	}

        std::copy(&buffer[i*utils::PAYLOAD_L], &buffer[i*utils::PAYLOAD_L+pay_len], message);
        
    	if (flagOut)
        	message[utils::PAYLOAD_L] = (unsigned char) (pay_len+100); //Add information of the size of the payload
        else
        	message[utils::PAYLOAD_L] = (unsigned char) pay_len;
        //ENCODING PACKET
        encoder->encode(message, packet); //use the encoder to encode the information
        
	printf("*** Sending packet number %i ***\n", i);
	utils::printPacket(packet, utils::CODE_L, 1);

	socket->write_socket(packet, utils::CODE_L, 0);
        
        
        //WAITING FOR ACK
        
        /* Wait for ACK */
        int ret;
        char *p_packet_ack = packet_ack;
        n = socket->read_non_blocking(p_packet_ack, utils::LEN_ACK, timeout, &ret);
        if (ret == 0) {
            // Timeout: send again
            printf("\n Timeout expired, resending \n");
            
            continue;
        }
        
        if (n == 0) {
            printf("\n Connection closed \n");
            break;  
        }
        
        
        //TODO: implement packetACK
        
        // Check ACK
        int c = Protocol::isAck(p_packet_ack);
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

