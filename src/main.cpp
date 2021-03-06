// main.cpp

#include <iostream>

#include "Protocol.hpp"
#include "Encoder.hpp"
#include "Compressor.hpp"
#include "Socket.hpp"
#include <csignal>

#include "utils.hpp"

using namespace std;
using namespace utils;

void termination_handler(int signal){
    utils::destroyGPIO();
	exit(1);
}

int main(int argc, char* argv[])
{
	signal(SIGTERM,termination_handler);
	utils::setupGPIO();
    /*if(argc < 2){
        cout << "Please provide arguments " << endl;
        return -1;
    }*/
    
    Compressor1 compressor(COMPRESSION_RATIO);
    EncoderRS<CODE_L, REDUNDANCY> encoder;

    if(utils::readTxRx()) {
        
        SocketRadio socket(1);
        //SocketUDP socket(1,(char *)"localhost");
        StopWait protocol(&compressor, &encoder, &socket);
        
        char* file_name;
        if(argc > 1){
            file_name = argv[1];
        }
        else{
            file_name = (char *)"/home/pi/MTP17Fall-GroupB/file.txt";
        }
        
        cout << "Input file name: " << file_name << endl;
        
        protocol.send_text(file_name);
        
    } else {
        
        SocketRadio socket(0);
        //SocketUDP socket(0,(char *)"localhost");
        StopWait protocol(&compressor, &encoder, &socket);
        
        protocol.receive_text();
    }
    utils::destroyGPIO();
    return 0;
}
