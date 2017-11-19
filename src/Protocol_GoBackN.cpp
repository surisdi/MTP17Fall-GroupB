//Protocol_GoBack_N.cpp

#include <iostream>
#include "Protocol.hpp"
#include "utils.hpp"

#include <cstdio>
#include <queue>
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

using namespace utils;

void printStatus(int currChunk, int num_chunk) {
	std::cout << "******** ";
	std::cout << "Chunk: " << currChunk + 1 << "/" << num_chunk;
	std::cout << " ********\n";
}

/***************** Derived Class Go Back N *****************/

GoBackN::GoBackN(Compressor *comp, Encoder *enc, Socket *sck):
Protocol(comp, enc, sck),
id_base(0),
id_send(0)
{
    clock_gettime(CLOCK_REALTIME, &clock_start);
    COUT<< "Go Back N created\n";
}

/*****************************************************/
/********************* Receiver **********************/
/*****************************************************/
int GoBackN::receive_text() {
    //COUT<< "Receiving text...\n";
    // https://stackoverflow.com/questions/8408776/queue-of-array-of-characters
    
    lastPacket = false;

    std::thread thread_queue(&GoBackN::receiverQueueThread, this);
    std::thread thread_processing(&GoBackN::receiverProcessingThread, this);
    while(!lastPacket){
        // Sleep?
    }
    thread_queue.join();
    thread_processing.join();

    //COUT<< "Finalized receiving!\n";

    return(1);    
}

// Receive messages and add them to a Queue
void GoBackN::receiverQueueThread(){
    while (!lastPacket) {
	bool result;
        byte packet[utils::CODE_L];
        result = socket->read_blocking(packet, utils::CODE_L);
        //utils::printPacket(packet, CODE_L,2);
        mtx2.lock();
	for(int i=1; i<=CODE_L;i++){
	  q.push(packet[i]);
        }
        mtx2.unlock();
    }
}

// Get messages form the Queue and process them
void GoBackN::receiverProcessingThread(){
    //Start the timer

    // Go Back N parameters
    int expected_id = 0;
    int received_id = -1;
    
    byte packet[CODE_L];
    byte corrected[CODE_L];
    
    compressor->startDecompress("output.txt");
    Chunk chunk;
    unsigned long buffSize;
    
    int counter = 0;
    byte ack_flags = 0x00;

    bool error;
    bool timer_started = false;
    unsigned int chunkSize = 0;
    byte dataSize = 0, flags = 0, previous = 0;
    unsigned long startTime;

    bool get_out = false;
    double num;

    std::vector<byte> buff;
    buff.reserve(sizeof(char)*utils::CHUNK_SIZE);
    
    while (!get_out) {
	while(!q.empty()){
        mtx2.lock();
	for(int i=1; i<=utils::CODE_L;i++){
          packet[i] = q.front();
	  //COUT << "Packet from queue:" << packet[i] << "\n";
          q.pop();
	}
	if(!timer_started){
	  startTime = millis();
	  timer_started = true;
	}	
        mtx2.unlock();
        
        //COUT<< "*** Received packet " << counter << " *** \n";
        //utils::printPacket(packet, utils::CODE_L, 2);
        
        error = encoder->decode(packet, corrected);
        
        if(error){
            //COUT << "Corrupted packet:  \n";
        }else{
            //COUT << "Corrected packet:  \n";

            //utils::printPacket(corrected, PAYLOAD_L_GBN, 2);
            flags = corrected[PAYLOAD_L_GBN+1];
            received_id = corrected[PAYLOAD_L_GBN];
        }
        //COUT << "received_id: " << received_id << ", expected_id: " << expected_id << "\n";

        if(!error && received_id==expected_id){
            mtx2.lock();
	    get_out = parseMessage(corrected, flags, previous, &chunkSize, &dataSize, &lastPacket);
            mtx2.unlock();
	    buff.insert(buff.end(), corrected, corrected + dataSize);
            if(lastPacket) {
                chunk.data = &buff[0];
                buffSize = buff.size();
                chunk.len = (unsigned long *)&buffSize;
                int z_result = compressor->decompressChunk(&chunk,  chunkSize);
                buff.clear();
            } else {
                previous = corrected[dataSize - 1];
            }
            counter++;


            // Acknowledge received packet
            ack_flags = utils::ack_gbn | expected_id;
            std::bitset<8> x(ack_flags);
            //COUT << "ack_flags: " << x << "\n";

            //utils::bsc(&ack_flags, 1, 0.1);
            num = (double)rand()/ (double) RAND_MAX;
            if(num < 0.5)
                socket->write_socket(&ack_flags, 1, 1);

            expected_id++; expected_id = expected_id%(utils::WINDOW_SIZE+1);

        }else{
            // Resend last ACK sent
            if(ack_flags != 0x00) // In case we have not sent any ACK
                socket->write_socket(&ack_flags, 1, 1);
        }
	}
    }

    for(int i = 0; i < N_RETRANSMIT_ACK; i++) {
        socket->write_socket(&ack_flags, 1, 1);
    }

    int ret = compressor->closeDecompress();
    unsigned long endTime = millis();
    COUT << (endTime - startTime) << '\n';
    if (ret) {
        COUT << "Error writing the file";
    } else {
        COUT << "Successful transmission - Congrats";
    }

}


bool GoBackN::parseMessage(byte *message, byte flags, byte previous, unsigned int *chunkSize, byte *dataSize, bool *lastPacket) {
    *dataSize = (flags & 0b00011111);
    *lastPacket = (flags & 0b01000000) >> 6;
    bool lastChunk = (flags & 0b10000000) >> 7;

    //utils::printPacket(message, utils::CODE_L, 2);
    bool get_out = false;
    if(*lastPacket) {
        if(lastChunk) {
            get_out = true;

            *chunkSize = 0x00000000;
            byte aux = previous;
            if(*dataSize > 1)
                aux = message[*dataSize -2];

            *chunkSize = aux;
            *chunkSize = (*chunkSize) << 8;
            aux = message[*dataSize-1];
            *chunkSize = (*chunkSize) | aux;

            *dataSize = *dataSize - 2;
        } else {
            *chunkSize = utils::CHUNK_SIZE;
        }
    }

    return get_out;
}

/*****************************************************/
/********************** Transmiter *******************/
/*****************************************************/
int GoBackN::send_text(char *text) {
    COUT<< "Sending text...\n";

    // --------- Compress File ----------- //
    Chunk *cmpFile;
    int num_chunk;
    cmpFile = compressor->compressFile(text, &num_chunk);
    //COUT << "Number of chunks: " << num_chunk << "\n";

    if (cmpFile == NULL) {
        //COUT << "COMPRESSION ERROR!!!";
        exit(1);
    }

    // --------- DECLARE VARIABLES ----------- //
    int first_p, last_p;

    bool can_send;
    bool timeout_expired, finished_text, finished_tx;
    timeout_expired = finished_text = finished_tx = false;

    byte *buffer;
    byte packet[utils::CODE_L];
    byte message[utils::CODE_L];
    std::memset(message, 0x00, utils::CODE_L);

    byte packets[utils::WINDOW_SIZE+1][utils::CODE_L];
    for (int i=0; i<utils::WINDOW_SIZE+1; i++){
        std::memset(packets[i], 0x00, utils::CODE_L);
    }


    int chunkLength, currChunk = 0;
    buffer = cmpFile[currChunk].data;
    chunkLength = (int)(*(cmpFile[currChunk].len));
    int extra = ((chunkLength % utils::PAYLOAD_L_GBN) != 0);

    int i = 0, k = 0; //position in the number of packets
    int g; //Packet to retransmit within the window
    
    // ACK parameters
    int aux;

    // ------------ INITIATE THREADS ------------- //
    std::thread thread_receive(&GoBackN::receiveThread, this);

    // ------------- START GO BACK N -------------//
    printStatus(currChunk, num_chunk);
    while (1) {

        // End of chunk
        if(i >= chunkLength/utils::PAYLOAD_L_GBN + extra){
            if(currChunk == num_chunk-1) {
                finished_text = true;
            } else {
                currChunk++;
                i = 0;
                buffer = cmpFile[currChunk].data;
                chunkLength = (int)(*(cmpFile[currChunk].len));
                extra = ((chunkLength % utils::PAYLOAD_L_GBN) != 0);
                printStatus(currChunk, num_chunk);
            }
        }

        can_send = false;
        timeout_expired = false;
        while(!can_send && !timeout_expired){
            mtx.lock();
            timeout_expired = GoBackN::timeoutExpired();
            aux = (id_send-id_base)%(utils::WINDOW_SIZE+1);
            if( aux < 0) aux+=utils::WINDOW_SIZE+1;
            can_send = aux < utils::WINDOW_SIZE;
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
            //COUT << "Timeout expired. Resend from " << first_p << " to " << last_p-1 << "\n";
            for(k=first_p; k < last_p; k++){
                //utils::bsc(packets[k],utils::CODE_L,0.0);
                g = k%(utils::WINDOW_SIZE + 1);
                double num = (double)rand()/ (double) RAND_MAX;
                if(num < 0.5)
                    socket->write_socket(packets[g], utils::CODE_L, 0);
                //usleep(2000);
            }
        }else if(can_send && !finished_text){

            GoBackN::createMessage(message, buffer, i, chunkLength, currChunk == (num_chunk -1));
            encoder->encode(message, packet);

            // Save packet in buffer
            std::copy(&packet[0], &packet[utils::CODE_L], packets[id_send]);

            //COUT << "*** Sending packet number " << i << " *** \n";
            //utils::printPacket(packet, utils::CODE_L, 2);

            // Send message
            //utils::bsc(packets[id_send],utils::CODE_L,0.0);
            double num = (double)rand()/ (double) RAND_MAX;
            if(num < 0.5)
                socket->write_socket(packets[id_send], utils::CODE_L, 0);

            mtx.lock();
            if (id_send == id_base) {
                clock_gettime(CLOCK_REALTIME, &clock_start);
                timer_running = true;
                //COUT << "Timeout started\n";
            }

            id_send++;
            id_send = id_send%(utils::WINDOW_SIZE+1);
            //COUT << "id_send updated to " << id_send << "\n";
            mtx.unlock();

            i++;
            //usleep(2000);
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

void GoBackN::receiveThread() {
    byte packet_ack[utils::LEN_ACK];
    byte *p_packet_ack = packet_ack;
    int ack_num;
    bool finished_p = false;
    int n, ret, isack;
    int timeout_receive = 1000;
    while(!finished_p){

        n = socket->read_non_blocking(p_packet_ack, utils::LEN_ACK, timeout_receive, &ret);
        if (ret == 0 || n == 0){
            // Do nothing, continue listening, but check finished_protocol
        } else {
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

// Needs to be locked
bool GoBackN::timeoutExpired(){
    int timeout = 50; // milliseconds
    clock_gettime(CLOCK_REALTIME,&clock_now);
    bool timeout_finished = (((clock_now.tv_sec*1000 + clock_now.tv_nsec/1000000) - 
                            (clock_start.tv_sec*1000 + clock_start.tv_nsec/1000000)) > timeout );
    return timer_running && timeout_finished;
}


void GoBackN::createMessage(byte *message, byte *buffer, int i, int len, bool isLast){
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

    byte lastByte = pay_len & 0b00011111;
    lastByte = lastByte | (flagOut << 6);
    lastByte = lastByte | (isLast << 7);

    message[utils::PAYLOAD_L_GBN + 1] = lastByte;

    // Introduce packet ID information
    mtx.lock();
    message[utils::PAYLOAD_L_GBN] =  (byte) id_send;
    mtx.unlock();
}


GoBackN::~GoBackN() {
    COUT<< "Go Back N destroyed...\n";
}
