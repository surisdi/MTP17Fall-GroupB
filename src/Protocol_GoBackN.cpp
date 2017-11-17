//Protocol_GoBack_N.cpp

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
#include <sys/time.h>
#include <map>
#include <bcm2835.h>
#define BUTTON_PI 1
/***************** Derived Class Go Back N *****************/

GoBackN::GoBackN(Compressor *comp, Encoder *enc, Socket *sck):
Protocol(comp, enc, sck)
{
    timer_running = false;
    clock_gettime(CLOCK_REALTIME, &clock_start);
    COUT<< "Go Back N created\n";
}

int GoBackN::receive_text() {
    
    COUT<< "Receiving text...\n";

    // Go Back N parameters
    int expected_id = 0;
    int received_id = -1;
    
    char packet[utils::CODE_L];
    char corrected[utils::CODE_L];
    
    int check = compressor->startDecompress("output.txt");
    Chunk chunk;
    unsigned long aux;
    
    int counter = 0;
    char ack_flags = 0x00;
    unsigned char aux_size;
    bool error;
    bool result;
    int decode_info;
    int chunk_size = utils::CHUNK_SIZE;
    unsigned int size;
    unsigned char my_aux;
    unsigned char aux_parser;
    unsigned char last_packet = 0;
    unsigned char last_chunk = 0;
    bool get_out = false;
    double num;
    //clock_gettime(CLOCK_REALTIME,&clock_now);

    //unsigned char *buffer = (unsigned char*)malloc(sizeof(char)*utils::CHUNK_SIZE);
    std::vector<unsigned char> buff;
    buff.reserve(sizeof(char)*utils::CHUNK_SIZE);

    while (!get_out) {
        //clock_gettime(CLOCK_REALTIME,&clock_start);
        //std::cout << "Diferencia temps en nanos: " << clock_start.tv_nsec - clock_now.tv_nsec << "\n";
        result = socket->read_blocking(packet, utils::CODE_L);
        //clock_gettime(CLOCK_REALTIME,&clock_now);
        if (!result) {
            COUT << "Connection closed \n";
            break;
        }
        
        COUT<< "*** Received packet " << counter << " *** \n";
        utils::printPacket(packet, utils::CODE_L, 2);
        
        error = encoder->decode(packet, corrected);
        
        if(error){
            COUT << "Corrupted packet:  \n";
            // socket->write_socket(&utils::nack, 1, 1);
        }else{
            COUT << "Corrected packet:  \n";

            utils::printPacket(corrected, utils::PAYLOAD_L_GBN, 2);
            size = (unsigned int) corrected[utils::PAYLOAD_L_GBN+1];
            received_id = (unsigned int) corrected[utils::PAYLOAD_L_GBN];
        }
        COUT << "received_id: " << received_id << ", expected_id: " << expected_id << "\n";

        if(!error && received_id==expected_id){

        	my_aux = (unsigned char)size;
        	aux_size = (my_aux & 0b00011111);
            last_packet = (my_aux & 0b01000000) >> 6;
            last_chunk = (my_aux & 0b10000000) >> 7;

            if (last_packet) {
            	if(last_chunk) {
            		get_out = true;

            		chunk_size = 0x00000000;
            		aux_parser = (unsigned char)corrected[aux_size-2];
            		aux_parser = aux_parser << 7;
            		chunk_size = aux_parser;
            		aux_parser = (unsigned char)corrected[aux_size-1];
            		chunk_size = chunk_size | aux_parser;
					buff.insert(buff.end(), corrected, corrected + aux_size-2);

	            	chunk.data = &buff[0];
	            	aux = buff.size();
	            	chunk.len = (unsigned long *)&aux;
	            	compressor->decompressChunk(&chunk, chunk_size);
            	}else{
            		buff.insert(buff.end(), corrected, corrected + aux_size);
	            	chunk.data = &buff[0];
	            	aux = buff.size();
	            	chunk.len = (unsigned long *)&aux;
	            	compressor->decompressChunk(&chunk, utils::CHUNK_SIZE);
            	}
            	buff.clear();
            }else {
            	buff.insert(buff.end(), corrected, corrected + aux_size);
            }
            counter++;

            // Acknowledge received packet
            ack_flags = utils::ack_gbn | expected_id;
            std::bitset<8> x(ack_flags);
            COUT << "ack_flags: " << x << "\n";

            // ----- GENERACIO ARTIFICIAL D'ERROR ----- //
            // Vigilar perque si el ACK no te redundancia (a part del CRC)
            // pot ser que faci ACK d'un altre paquet... que passaria?
            //utils::bsc(&ack_flags,utils::CODE_L,0.1);
            //double num = (double)rand()/ (double) RAND_MAX;
            if(num<1)
                socket->write_socket(&ack_flags, 1, 1);

            expected_id++; expected_id = expected_id%(utils::WINDOW_SIZE+1);

        }else{
            // Resend last ACK sent
            if(ack_flags != 0x00) // In case we have not sent any ACK
                socket->write_socket(&ack_flags, 1, 1);
        }
    }
    
    int ret = compressor->closeDecompress();
    if (ret) {
    	COUT << "Error writing the file";
        return 0;
    } else {
    	return 1;
    }
    
}

void GoBackN::receiveThread() {
    char packet_ack[utils::LEN_ACK];
    char *p_packet_ack = packet_ack;
    int ack_num;
    bool finished_p = false;
    int n, ret, isack;
    int timeout_receive = 1000;
    while(!finished_p){
        // We do it with a timeout so that periodically it checks if the program has ended
        // Wait for ack
	//COUT << "*** About to read ACK ***\n";
        n = socket->read_non_blocking(p_packet_ack, utils::LEN_ACK, timeout_receive, &ret);
        //COUT << "*** Have read ACK ***\n";
	if (ret == 0 || n == 0){
            // Do nothing, continue listening, but check finished_protocol
        }else{
	    COUT << "ACK received\n";
            isack = isAck(p_packet_ack);
            if (isack == 1){
                ack_num = ((*p_packet_ack) & 0x1F);
                COUT << "New ack received confirming " << ack_num << "\n";
                mtx.lock();
                id_base = ack_num + 1;id_base=id_base%(utils::WINDOW_SIZE+1);
                if(id_send == id_base){
                    timer_running = false;
                }else{
                    timer_running = true;
                    clock_gettime(CLOCK_REALTIME, &clock_start);
                }
                mtx.unlock();
            }
        }

        mtx.lock();
        finished_p = finished_protocol;
        mtx.unlock();
    }   
}

/*void GoBackN::interruption(void) {
	n = socket->read_non_blocking(p_packet_ack, utils::LEN_ACK, timeout_receive, &$
        if (ret == 0 || n == 0){
            // Do nothing, continue listening, but check finished_protocol
        }else{
            COUT << "ACK received\n";
            isack = isAck(p_packet_ack);
            if (isack == 1){
                ack_num = ((*p_packet_ack) & 0x1F);
                COUT << "New ack received confirming " << ack_num << "\n";
                mtx.lock();
                id_base = ack_num + 1;
                if(id_send == id_base){
                    timer_running = false;
                }else{
                    timer_running = true;
                    clock_gettime(CLOCK_REALTIME, &clock_start);
                }
                mtx.unlock();
            }
        }

}*/

bool GoBackN::timeoutExpired(){
    // Has to be used inside a lock()
    int timeout = 50; // milliseconds
    clock_gettime(CLOCK_REALTIME,&clock_now);
    bool timeout_finished = (((clock_now.tv_sec*1000 + clock_now.tv_nsec/1000000) - 
                            (clock_start.tv_sec*1000 + clock_start.tv_nsec/1000000)) > timeout );
    return timer_running && timeout_finished;
}

void GoBackN::createMessage(char *message, char *buffer, int i, int len, bool isLast){
    int pay_len;
    bool flagOut = false;

    if ( (i + 1) * utils::PAYLOAD_L_GBN > len ) {
        pay_len = len - ( i * utils::PAYLOAD_L_GBN );
    } else {
        pay_len = utils::PAYLOAD_L_GBN;
    }

    // Introduce packet length information
    if(i*utils::PAYLOAD_L_GBN + pay_len == len){
        flagOut = true;
    }
    std::copy(&buffer[i*utils::PAYLOAD_L_GBN], &buffer[i*utils::PAYLOAD_L_GBN+pay_len], message);

    char lastByte = pay_len & 0b00011111;
    lastByte = lastByte | (flagOut << 6);
    lastByte = lastByte | (isLast << 7);

    message[utils::PAYLOAD_L_GBN + 1] = lastByte;

    // Introduce packet ID information
    mtx.lock();
    message[utils::PAYLOAD_L_GBN] =  (unsigned char) id_send;
    mtx.unlock();
}


int GoBackN::send_text(char *text) {
    COUT<< "Sending text...\n";

    // Compress file before starting transmission
    Chunk *cmpFile;
    int num_chunk;
    cmpFile = compressor->compressFile(text, &num_chunk);
    COUT << "Number of chunks: " << num_chunk << "\n";

    if (cmpFile == NULL) {
    	COUT << "COMPRESSION ERROR!!!";
		exit(1);
    }
    // wiringPiSetup();
    //wiringPiISR(BUTTON_PIN, INT_EDGE_FALLING, &GoBackN::interruption);

    // --------- DECLARE VARIABLES ----------- //
    // Go back N variables
    id_send = 0; // Next packet to be sent
    id_base = 0; // Expected packet to be acknowledged
    int id_ack; // Variable to save the acknowledged packet ID

    // Auxiliar variables
    int currChunk = 0;
    int len;
    int size;
    int first_p,last_p;
    bool can_send;
    bool timeout_expired = false;
    char *buffer;
    unsigned char *bufferx;
    bool finished_text = false;
    bool finished_tx = false;
    char packet[utils::CODE_L];
    char message[utils::CODE_L];

    buffer = (char *)cmpFile[currChunk].data;
    len = (int)(*(cmpFile[currChunk].len));
    char packets[utils::WINDOW_SIZE+1][utils::CODE_L];
    for (int i=0; i<utils::WINDOW_SIZE+1; i++){
        std::memset(packets[i], 0x00, utils::CODE_L);
    }

    std::memset(message, 0x00, utils::CODE_L);
    int i = 0, k=0; //position in the number of packets
    int extra = ((len % utils::PAYLOAD_L_GBN) != 0); //extra we need for the last packet
    int pay_len; //actual lenght of the payload of the current packet
    
    // ACK parameters
    bool rec_ack;
    int flagOut = 0;
    int aux;

    // ------------ INITIATE THREADS ------------- //

    // Initiate thread receiving ACK
    std::thread thread_receive(&GoBackN::receiveThread, this);

    // ------------- START GO BACK N -------------//
    std::cout << "******** ";
	std::cout << "Chunk: " << currChunk + 1 << "/" << num_chunk;
	std::cout << " ********\n";
    while (1) {

    	if(i >= len/utils::PAYLOAD_L_GBN + extra){
    		if(currChunk == num_chunk-1) {
    			finished_text = true;
    		} else {
    			currChunk++;
    			i = 0;
    			std::cout << "******** ";
    			std::cout << "Chunk: " << currChunk + 1 << "/" << num_chunk;
    			std::cout << " ********\n";
    			buffer = (char*)cmpFile[currChunk].data;
    			len = (int)(*(cmpFile[currChunk].len));
    			extra = ((len % utils::PAYLOAD_L_GBN) != 0);
    		}
    	}

		can_send = false;
		timeout_expired = false;
		while(!can_send && !timeout_expired){
			mtx.lock();
			timeout_expired = GoBackN::timeoutExpired();
			aux=(id_send-id_base)%(utils::WINDOW_SIZE+1);
			if( aux < 0) aux+=utils::WINDOW_SIZE+1;
			can_send = aux < utils::WINDOW_SIZE;
			//COUT << aux << "\n";
			mtx.unlock();
		}
		//Reenviar
		if (timeout_expired){
			mtx.lock();
			clock_gettime(CLOCK_REALTIME, &clock_start);
			timer_running = true;
			first_p = id_base; last_p = id_send;
			mtx.unlock();

			if (first_p > last_p) last_p += utils::WINDOW_SIZE + 1; // Because of the module WSIZE+1
			// Resend all non acknowledged packets
			COUT << "Timeout expired. Resend from " << first_p << " to " << last_p-1 << "\n";
			for(k=first_p; k < last_p; k++){
				//COUT<< "*** Resending packet number " << k << " *** \n";

				// ----- GENERACIO ARTIFICIAL D'ERROR ----- //
				//utils::bsc(packets[k],utils::CODE_L,0.0);
				double num = (double)rand()/ (double) RAND_MAX;
				//if(num<1)
					//clock_gettime(CLOCK_REALTIME,&clock_now);
					socket->write_socket(packets[k], utils::CODE_L, 0);
					//clock_gettime(CLOCK_REALTIME,&clock_start);
					//COUT << "TEMPS LIMITANT " << clock_start.tv_nsec - clock_now.tv_nsec << "\n";

				// This usleep is important to give time to the rx to process the
				// previous packet. Experimentally, the time the rx needs is less than
				// 300 ms when there are no prints, and less than 600 ms in general
				// with prints. 1000 gives some margin (we have to add the processing time
				// of the TX, which in this usleep is very few but in the other one is bigger).
				usleep(2000);
			}
		}else if(can_send && !finished_text){ //Enviar packets
			// Create message
			GoBackN::createMessage(message, buffer, i, len, currChunk == (num_chunk -1));

			// Encode packet
			encoder->encode(message, packet); //use the encoder to encode the information

			// Save packet in buffer
			// We have to copy the packet, if not, the next packet will overwrite (same memory address)
			std::copy(&packet[0], &packet[utils::CODE_L], packets[id_send]);

			COUT << "*** Sending packet number " << i << " *** \n";
			utils::printPacket(packet, utils::CODE_L, 2);

			// Send message
			// ----- GENERACIO ARTIFICIAL D'ERROR ----- //
			utils::bsc(packets[id_send],utils::CODE_L,0.0);
			double num = (double)rand()/ (double) RAND_MAX;
			if(num<1)
				socket->write_socket(packets[id_send], utils::CODE_L, 0);

			mtx.lock();
			if (id_send == id_base){
				clock_gettime(CLOCK_REALTIME,&clock_start);
				timer_running = true;
				COUT << "Timeout started\n";
			}
			id_send++; id_send = id_send%(utils::WINDOW_SIZE+1);
			COUT << "id_send updated to " << id_send << "\n";
			mtx.unlock();

			usleep(2000);

			i++;
		}
		mtx.lock();
		finished_tx = finished_text && id_send==id_base;
		mtx.unlock();
		if (finished_tx) break;

    }
    COUT<< "Finished transmitting!\n";
    mtx.lock();
    finished_protocol = true;
    mtx.unlock();
    thread_receive.join();
    return 0;
}

GoBackN::~GoBackN() {
    COUT<< "Go Back N destroyed...\n";
    
}
