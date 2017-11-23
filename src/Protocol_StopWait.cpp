//Protocol_StopWait.cpp

#include <iostream>
#include "Protocol.hpp"
#include "Compressor.hpp"
#include "utils.hpp"

#include <cstdio>
#include <cstdlib> // for the random
#include <unistd.h>
#include <cstring>
#include <bitset>
#include <thread>
#include <mutex>
#include <sys/time.h>


void printStatus(int currChunk, int num_chunk) {
    std::cout << "******** ";
    std::cout << "Chunk: " << currChunk + 1 << "/" << num_chunk;
    std::cout << " ********\n";
}

/***************** Derived Class StopWait *****************/

StopWait::StopWait(Compressor *comp, Encoder *enc, Socket *sck):
Protocol(comp, enc, sck)
{
    COUT<< "Stop and wait created\n";
}


bool StopWait::parseMessage(byte *message, byte flags, byte previous, unsigned int *chunkSize, byte *dataSize, bool *lastPacket, bool *correctID, int rnext) {
    
    *dataSize = (flags & 0b00011111);
    bool id = (flags & 0b00100000) >> 5;
    *lastPacket = (flags & 0b01000000) >> 6;
    bool lastChunk = (flags & 0b10000000) >> 7;

    //utils::printPacket(message, utils::CODE_L, 2);
    *correctID = (id == rnext);
    bool get_out = false;
    if (*correctID){
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
    }
    return get_out;
}


int StopWait::receive_text() {

    COUT<< "Receiving text...\n";
    utils::stayRed();
	
    byte packet[utils::CODE_L];
    byte corrected[utils::CODE_L];

    compressor->startDecompress("output.txt");
    Chunk chunk;
    unsigned long buffSize;
    unsigned long timer;
    unsigned int chunkSize = 0;
    byte dataSize = 0, flags = 0, previous = 0;

    bool lastPacket = false, get_out = false;
    bool correctID;
    int counter = 0;
    byte ack_flags = 0x00;

    bool error;
    
    bool rnext=0;
    std::vector<byte> buff;
    buff.reserve(sizeof(char)*utils::CHUNK_SIZE);
    bool ha_entrat = false;
    while (!get_out) {
        
        socket->read_blocking(packet, utils::CODE_L);
        if(!ha_entrat){
            utils::stopRed();
	        timer = millis();
	        ha_entrat = true;
	    }
        COUT << "*** Received packet " << counter << " *** \n";
        //utils::printPacket(packet, utils::CODE_L, 2);

        error = encoder->decode(packet, corrected);
	
        if(error){
            COUT << "Paquet incorrecte:  \n";
            //socket->write_socket(&utils::nack, 1, 1);
        }else{
            COUT << "Corrected packet:  \n";
	    flags = corrected[utils::PAYLOAD_L];
       	    get_out = parseMessage(corrected, flags, previous, &chunkSize, &dataSize, &lastPacket, &correctID, rnext);
	    
            if (correctID){
		COUT << "CorrectID\n";
            	//utils::printPacket(corrected, utils::PAYLOAD_L, 2);
	    	rnext = 1-rnext;	    
	    	counter++;

           	buff.insert(buff.end(), corrected, corrected + dataSize);
            	if(lastPacket) {
               	    chunk.data = &buff[0];
                    buffSize = buff.size();
                    chunk.len = (unsigned long *)&buffSize;
                    int z_result = compressor->decompressChunk(&chunk,  chunkSize);
                    utils::blinkGreen();
                    buff.clear();
             	} else {
                    previous = corrected[dataSize - 1];
                }
	    }
	 }
	 // Send ack 
         ack_flags = utils::ack_sw | rnext;
         std::bitset<8> x(ack_flags);
         COUT<< "ack_flags: " << x << "\n";
         socket->write_socket(&ack_flags, 1, 1);
    }

    int ret = compressor->closeDecompress();
    std::cout << (millis()-timer) << "\n";
    if (ret) {
        COUT << "Error writing the file";
        return 0;
    } else {
        utils::stayGreen();
        return 1;
    }

}

void StopWait::createMessage(byte *message, byte *buffer, int i, int len, bool isLast, bool flagSW){

    int pay_len;
    bool flagOut = false;

    if ( (i + 1) * utils::PAYLOAD_L >= len ) {
        pay_len = len - ( i * utils::PAYLOAD_L );
	flagOut = true;
    } else {
        pay_len = utils::PAYLOAD_L;
    }
/*

    if(i*utils::PAYLOAD_L + pay_len == len){
        flagOut = true;
    }
*/
    COUT << "pay_len: " << pay_len << "\n";

    std::copy(&buffer[i*utils::PAYLOAD_L], &buffer[i*utils::PAYLOAD_L+pay_len], message);

    byte lastByte = pay_len & 0b00011111;
    lastByte = lastByte | (flagSW << 5);
    lastByte = lastByte | (flagOut << 6);
    lastByte = lastByte | (isLast << 7);

    message[utils::PAYLOAD_L] = lastByte;
}

int StopWait::send_text(char *text) {

    COUT<< "Sending text...\n";

    // --------- Compress File ----------- //
    Chunk *cmpFile;
    int num_chunk;
    cmpFile = compressor->compressFile(text, &num_chunk);
    COUT << "Number of chunks: " << num_chunk << "\n";

    if (cmpFile == NULL) {
        COUT << "COMPRESSION ERROR!!!";
        exit(1);
    }

    byte *buffer;
    byte packet[utils::CODE_L];
    byte message[utils::CODE_L];
    std::memset(message, 0x00, utils::CODE_L);

    int chunkLength, currChunk = 0;
    buffer = cmpFile[currChunk].data;
    chunkLength = (int)(*(cmpFile[currChunk].len));
    int extra = ((chunkLength % utils::PAYLOAD_L) != 0);

    int i = 0; //position in the number of packets

    /* ACK parameters */
    int timeout = 50; // milliseconds
    int packet_id;
    bool slast=0;
    
    int result, n;

    byte packet_ack[utils::LEN_ACK];
    byte *p_packet_ack = packet_ack;
    int pac_num;
    bool finished_p = false;
    int ret, isack;
    int timeout_receive=50;

    // Initiate thread receive
    //std::thread thread_receive(&StopWait::receiveThread, this);

    printStatus(currChunk, num_chunk);
    while (1) {

        // End of chunk
        if(i >= chunkLength/utils::PAYLOAD_L + extra){
            if(currChunk == num_chunk-1) {
                utils::blinkRed();
                break;
            } else {
                currChunk++;
                i = 0;
                buffer = cmpFile[currChunk].data;
                chunkLength = (int)(*(cmpFile[currChunk].len));
                extra = ((chunkLength % utils::PAYLOAD_L) != 0);
                utils::blinkGreen();
                printStatus(currChunk, num_chunk);
            }
        }

        // Create message
        
        StopWait::createMessage(message, buffer, i, chunkLength, currChunk == (num_chunk-1), slast);

        // Encode packet
        encoder->encode(message, packet); //use the encoder to encode the information

    	COUT<< "*** Sending packet number " << i << " *** \n";;
    	//utils::printPacket(packet, utils::CODE_L, 2);

    	socket->write_socket(packet, utils::CODE_L, 0);
        /* Wait for ACK */
            
       	
        // We do it with a timeout so that periodically it checks if the program has ended	
        // Wait for ack
        result = socket->read_non_blocking(p_packet_ack, utils::LEN_ACK, timeout_receive, &ret);
        if (ret == 0){
            // Retransmit
	    COUT << "timeout\n";
        }else{
            isack = isAck(p_packet_ack);
            pac_num = ((*p_packet_ack) & 0x01);
	    if (isack && pac_num==1-slast){
		slast=1-slast;
		i++;
		COUT << "ACK received with packet ID" << pac_num << "\n";
            }else{
		COUT << "Wrong ack received\n";
	    }
        }
    }
    COUT<< "Finished transmitting!\n";
    utils::stayGreen();
    return 0;
}

StopWait::~StopWait() {
    COUT<< "Stop and Wait destroyed...\n";

}
