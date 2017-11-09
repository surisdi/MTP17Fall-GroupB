
//Protocol.cpp

#include <iostream>
#include "Protocol.hpp"
#include "utils.hpp"

#include <cstdio>
#include <cstdlib> // for the random
#include <unistd.h>
#include <cstring>
#include <bitset>
#include <thread>
#include <mutex>
#include <ctime>

#include <bitset>
#include <map>

/***************** Base Class Protocol *****************/

Protocol::Protocol(Compressor *comp, Encoder *enc, Socket *sck):
compressor(comp),
encoder(enc),
socket(sck) {
    finished_protocol = false;
}

int Protocol::isAck(const char* r_ack) {
        //std::bitset<sizeof(char)> bs(r_ack);
    //Forgive me god, for i have sinned
	int n_ones = 0;
	int n_zeros = 0;

    //std::bitset<8> x(*r_ack);
    //std::cout << "ack packet: " << x << std::endl;
    // The 5 lowest bits are for flags (FOR PACKET ID)
	for (int k = 5; k < 8; k++) {
		if(((*r_ack) & (0x01 << k)) != 0) {
			n_ones++;
		}
	}

	if (n_ones > 1) {
		return 1;
	}else if(n_ones < 2) {
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
        std::cout << "Inside while..." << std::endl;
        result = socket->read_blocking(packet, utils::CODE_L);
        
        if (!result) {
            printf("Connection closed \n");
            break;
        }
        
        std::cout << "*** Received packet " << counter << " *** " << std::endl;
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
            std::cout << "Size: " << +size << std::endl;

            fwrite(corrected, sizeof(char), size, outputFile);
            counter++;

            char ack_flags = utils::ack | counter%2;
            std::bitset<8> x(ack_flags);
            std::cout << "ack_flags: " << x << std::endl;
            socket->write_socket(&ack_flags, 1, 1);
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

void StopWait::ReceiveThread() {
    char packet_ack[utils::LEN_ACK];
    char *p_packet_ack = packet_ack;
    int pac_num;
    bool finished_p = false;
    int n, ret, isack;
    int timeout_receive = 1000;
    while(!finished_p){
        pac_num = -1;
        // We do it with a timeout so that periodically it checks if the program has ended
        // Wait for ack
        n = socket->read_non_blocking(p_packet_ack, utils::LEN_ACK, timeout_receive, &ret);
        if (ret == 0){
            // Do nothing, continue listening, but check finished_protocol
        }else{
            if (n==0){
                // Something?
            }else{
                isack = isAck(p_packet_ack);
                pac_num = ((*p_packet_ack) & 0x01);
                std::cout << "ACK received" << std::endl;
            }       
        }

        mtx.lock();
        finished_p = finished_protocol;
        flag_ack = isack;
        flag_pac_num = pac_num;
        mtx.unlock();
    }   
}

int StopWait::send_text(char *text) {

    // Initiate thread receive
    std::thread thread_receive(&StopWait::ReceiveThread, this);

    
    std::cout << "Sending text..." << std::endl;
    
    int len;
    int size;
    char *buffer = utils::read_text(text, &len);
    
    //char data[utils::PAYLOAD_L];
    char packet[utils::CODE_L];
    char message[utils::CODE_L];
    
    std::memset(message, 0x00, utils::CODE_L);
    
    int i = 0; //position in the number of packets
    int extra = ((len % utils::PAYLOAD_L) != 0); //extra we need for the last packet
    int pay_len; //actual lenght of the payload of the current packet
    
    /* ACK parameters */
    int timeout = 50; // milliseconds
    clock_t start_clock;
    int packet_id;
    bool rec_ack;

    int flagOut = 0;
    while (i < len / utils::PAYLOAD_L + extra) {
        
        if ( (i + 1) * utils::PAYLOAD_L > len ) {
            pay_len = len - ( i * utils::PAYLOAD_L );
        } else {
            pay_len = utils::PAYLOAD_L;
        }

    	if(i*utils::PAYLOAD_L + pay_len == len){
    		flagOut = 1;
    	}

        std::copy(&buffer[i*utils::PAYLOAD_L], &buffer[i*utils::PAYLOAD_L+pay_len], message);
        
    	if (flagOut)
        	message[utils::PAYLOAD_L] = (unsigned char) (pay_len+100); //Add information of the size of the payload
        else
        	message[utils::PAYLOAD_L] = (unsigned char) pay_len;
        //ENCODING PACKET
        encoder->encode(message, packet); //use the encoder to encode the information
            
    	std::cout << "*** Sending packet number " << i << " *** " << std::endl;;
    	utils::printPacket(packet, utils::CODE_L, 1);

    	socket->write_socket(packet, utils::CODE_L, 0);
            
        /* Wait for ACK */
        start_clock = clock();
        rec_ack = false;
        while((clock()-start_clock)/(CLOCKS_PER_SEC/1000) < timeout ){
            mtx.lock();
            if (flag_ack != 0){
                if (flag_ack == 1){                    
                    packet_id = flag_pac_num;
                    rec_ack = true;
                }
                flag_ack = 0; //restart flag
                mtx.unlock();
                break;
            }
            mtx.unlock();
        }
        if (!rec_ack){
            std::cout << "Timeout expired or NACK received: resending" << std::endl;
        }else{
            std::cout << "ACK received with packet ID " << packet_id << std::endl;
            i++;
        }

    }
    std::cout << "Finished transmitting!" << std::endl;
    mtx.lock();
    finished_protocol = true;
    mtx.unlock();
    thread_receive.join();
    return 0;
}

StopWait::~StopWait() {
    std::cout << "Stop and Wait destroyed..." << std::endl;
    
}


/***************** Derived Class Go Back N *****************/

GoBackN::GoBackN(Compressor *comp, Encoder *enc, Socket *sck):
Protocol(comp, enc, sck)
{
    std::cout << "Stop and wait created" << std::endl;
}

int GoBackN::receive_text() {
    
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
        std::cout << "Inside while..." << std::endl;
        result = socket->read_blocking(packet, utils::CODE_L);
        
        if (!result) {
            printf("Connection closed \n");
            break;
        }
        
        std::cout << "*** Received packet " << counter << " *** " << std::endl;
        utils::printPacket(packet, utils::CODE_L, 1);
        
        error = encoder->decode(packet, corrected);
        
        if(error){
            printf("Paquet incorrecte:  \n");
            socket->write_socket(&utils::nack, 1, 1);
        }else{
            printf("Corrected packet:  \n");

            utils::printPacket(corrected, utils::PAYLOAD_L_GBN, 1);
            size = (unsigned int) corrected[utils::PAYLOAD_L_GBN+1];

            if (size > utils::PAYLOAD_L_GBN){
                size = size - 100;
                last_packet = true;
            }
            std::cout << "Size: " << +size << std::endl;

            fwrite(corrected, sizeof(char), size, outputFile);
            counter++;

            char ack_flags = utils::ack | counter%2;
            std::bitset<8> x(ack_flags);
            std::cout << "ack_flags: " << x << std::endl;
            socket->write_socket(&ack_flags, 1, 1);
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

void GoBackN::ReceiveThread() {
    char packet_ack[utils::LEN_ACK];
    char *p_packet_ack = packet_ack;
    int pac_num;
    bool finished_p = false;
    int n, ret, isack;
    int timeout_receive = 1000;
    while(!finished_p){
        pac_num = -1;
        // We do it with a timeout so that periodically it checks if the program has ended
        // Wait for ack
        n = socket->read_non_blocking(p_packet_ack, utils::LEN_ACK, timeout_receive, &ret);
        if (ret == 0){
            // Do nothing, continue listening, but check finished_protocol
        }else{
            if (n==0){
                // Something?
            }else{
                isack = isAck(p_packet_ack);
                pac_num = ((*p_packet_ack) & 0x01);
                std::cout << "ACK received" << std::endl;
            }       
        }

        mtx.lock();
        finished_p = finished_protocol;
        flag_ack = isack;
        flag_pac_num = pac_num;
        mtx.unlock();
    }   
}

struct Packet
{
    //char *pac[utils::CODE_L];
    char *pac;
};

int GoBackN::send_text(char *text) {

    // Go back N variables

    int id_send = 0;
    int id_expected = 0; // expected packet to be acknowledged

    int id_receive; // to save the acknowledged packet ID

    // Initiate thread receive
    std::thread thread_receive(&GoBackN::ReceiveThread, this);
    
    std::cout << "Sending text..." << std::endl;
    
    int len;
    int size;
    char *buffer = utils::read_text(text, &len);

    char packet[utils::CODE_L];
    char message[utils::CODE_L];

    std::map<int,Packet> packets;

    std::memset(message, 0x00, utils::CODE_L);
    
    int i = 0; //position in the number of packets
    int extra = ((len % utils::PAYLOAD_L_GBN) != 0); //extra we need for the last packet
    int pay_len; //actual lenght of the payload of the current packet
    
    /* ACK parameters */
    int timeout = 50; // milliseconds
    bool rec_ack;
    std::map<int,clock_t> start_clocks;

    int flagOut = 0;

    while (i < len / utils::PAYLOAD_L_GBN + extra) {
        
        if ( (i + 1) * utils::PAYLOAD_L_GBN > len ) {
            pay_len = len - ( i * utils::PAYLOAD_L_GBN );
        } else {
            pay_len = utils::PAYLOAD_L_GBN;
        }

        if(i*utils::PAYLOAD_L_GBN + pay_len == len){
            flagOut = 1;
        }

        std::copy(&buffer[i*utils::PAYLOAD_L_GBN], &buffer[i*utils::PAYLOAD_L_GBN+pay_len], message);
        
        if (flagOut)
            message[utils::PAYLOAD_L_GBN+1] = (unsigned char) (pay_len+100); //Add information of the size of the payload
        else
            message[utils::PAYLOAD_L_GBN+1] = (unsigned char) pay_len;

        // Introduce packet ID information
        message[utils::PAYLOAD_L_GBN] =  (unsigned char) id_send;

        //ENCODING PACKET
        encoder->encode(message, packet); //use the encoder to encode the information

        // Save packet in buffer
        // AQUEST BUFFER ES NECESSARI PERQUE QUAN PASSI EL TIMEOUT HEM DE REENVIAR ELS SEUS MISSATGES
        packets[id_send] = {packet}; 

        /*if (id_send < id_expected + utils::WINDOW_SIZE){

        }*/
            
        std::cout << "*** Sending packet number " << i << " *** " << std::endl;;
        utils::printPacket(packet, utils::CODE_L, 1);

        socket->write_socket(packets[id_send].pac, utils::CODE_L, 0);
            
        /* Wait for ACK */
        start_clocks[id_send] = clock();
        rec_ack = false;
        // AQUI TINDREM ELS TIMEOUTS DE CADA PAQUET, PERO NOMES CALDRA QUE COMPROVEM EL TIMEOUT DEL PAQUET MES ANTIC. QUAN AQUEST SIGUI ACK-EJAT EL SEGUENT PASSARA A SER EL IMPORTANT
        while((clock()-start_clocks[id_send])/(CLOCKS_PER_SEC/1000) < timeout ){
            mtx.lock();
            if (flag_ack != 0){
                if (flag_ack == 1){                    
                    id_receive = flag_pac_num;
                    rec_ack = true;
                }
                flag_ack = 0; //restart flag
                mtx.unlock();
                break;
            }
            mtx.unlock();
        }
        if (!rec_ack){
            std::cout << "Timeout expired or NACK received: resending" << std::endl;
        }else{
            std::cout << "ACK received with packet ID " << packet_id_receive << std::endl;
            i++;
        }

    }
    std::cout << "Finished transmitting!" << std::endl;
    mtx.lock();
    finished_protocol = true;
    mtx.unlock();
    thread_receive.join();
    return 0;
}

GoBackN::~GoBackN() {
    std::cout << "Stop and Wait destroyed..." << std::endl;
    
}
