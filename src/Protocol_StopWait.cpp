//Protocol_StopWait.cpp

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

/***************** Derived Class StopWait *****************/

StopWait::StopWait(Compressor *comp, Encoder *enc, Socket *sck):
Protocol(comp, enc, sck)
{
    COUT<< "Stop and wait created\n";
}

int StopWait::receive_text() {

    COUT<< "Receiving text...\n";

    byte packet[utils::CODE_L];
    byte corrected[utils::CODE_L];

    FILE *outputFile = fopen("output.txt", "w+b");

    int counter = 0;
    bool error;
    bool result;
    unsigned int size;
    bool last_packet = false;

    while (!last_packet) {
        COUT<< "Inside while...\n";
        result = socket->read_blocking(packet, utils::CODE_L);

        if (!result) {
            COUT << "Connection closed \n";
            break;
        }

        COUT << "*** Received packet " << counter << " *** \n";
        utils::printPacket(packet, utils::CODE_L, 2);

        error = encoder->decode(packet, corrected);

        if(error){
            COUT << "Paquet incorrecte:  \n";
            socket->write_socket(&utils::nack, 1, 1);
        }else{
            COUT << "Corrected packet:  \n";

            utils::printPacket(corrected, utils::PAYLOAD_L, 2);
            size = (unsigned int) corrected[utils::PAYLOAD_L];

            if (size > utils::PAYLOAD_L){
                size = size - 100;
                last_packet = true;
            }
            COUT<< "Size: " << size << "\n";

            fwrite(corrected, sizeof(char), size, outputFile);
            counter++;

            byte ack_flags = utils::ack_sw | counter%2;
            std::bitset<8> x(ack_flags);
            COUT<< "ack_flags: " << x << "\n";
            socket->write_socket(&ack_flags, 1, 1);
        }

    }

    if (outputFile != NULL) {
        fclose(outputFile);
        return 1;
    } else {
        COUT << "Error writing the file";
        return 0;
    }

}

void StopWait::receiveThread() {
    byte packet_ack[utils::LEN_ACK];
    byte *p_packet_ack = packet_ack;
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
                COUT<< "ACK received\n";
            }
        }

        mtx.lock();
        finished_p = finished_protocol;
        flag_ack = isack;
        flag_pac_num = pac_num;
        mtx.unlock();
    }
}

void StopWait::createMessage(byte *message, byte *buffer, int i, int len){

    bool flagOut = false;
    int pay_len;

    if ( (i + 1) * utils::PAYLOAD_L > len ) {
        pay_len = len - ( i * utils::PAYLOAD_L );
    } else {
        pay_len = utils::PAYLOAD_L;
    }

    if(i*utils::PAYLOAD_L + pay_len == len){
        flagOut = true;
    }

    COUT << "pay_len: " << pay_len << "\n";

    std::copy(&buffer[i*utils::PAYLOAD_L], &buffer[i*utils::PAYLOAD_L+pay_len], message);

    if (flagOut)
        message[utils::PAYLOAD_L] = (byte) (pay_len+100); //Add information of the size of the payload
    else
        message[utils::PAYLOAD_L] = (byte) pay_len;
}

int StopWait::send_text(char *text) {

    // Initiate thread receive
    std::thread thread_receive(&StopWait::receiveThread, this);


    COUT<< "Sending text...\n";

    int len;
    byte *buffer = utils::read_text(text, &len);

    //char data[utils::PAYLOAD_L];
    byte packet[utils::CODE_L];
    byte message[utils::CODE_L];

    std::memset(message, 0x00, utils::CODE_L);

    int i = 0; //position in the number of packets
    int extra = ((len % utils::PAYLOAD_L) != 0); //extra we need for the last packet

    /* ACK parameters */
    int timeout = 50; // milliseconds
    int packet_id;
    bool rec_ack;

    while (i < len / utils::PAYLOAD_L + extra) {
        // Create message
        StopWait::createMessage(message, buffer, i, len);

        // Encode packet
        encoder->encode(message, packet); //use the encoder to encode the information

    	COUT<< "*** Sending packet number " << i << " *** \n";;
    	utils::printPacket(packet, utils::CODE_L, 2);

    	socket->write_socket(packet, utils::CODE_L, 0);

        /* Wait for ACK */
        clock_gettime(CLOCK_REALTIME, &clock_start);
        rec_ack = false;
        while(1){
            clock_gettime(CLOCK_REALTIME,&clock_now);
            if(((clock_now.tv_sec*1000 + clock_now.tv_nsec/1000000) - (clock_start.tv_sec*1000 + clock_start.tv_nsec/1000000)) > timeout) break;
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
            COUT<< "Timeout expired or NACK received: resending\n";
        }else{
            COUT<< "ACK received with packet ID " << packet_id << "\n";
            i++;
        }

    }
    COUT<< "Finished transmitting!\n";
    mtx.lock();
    finished_protocol = true;
    mtx.unlock();
    thread_receive.join();
    return 0;
}

StopWait::~StopWait() {
    COUT<< "Stop and Wait destroyed...\n";

}
