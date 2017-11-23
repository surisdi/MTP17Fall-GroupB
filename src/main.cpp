// main.cpp

#include <iostream>

#include "Protocol.hpp"
#include "Encoder.hpp"
#include "Compressor.hpp"
#include "Socket.hpp"

#include "utils.hpp"

using namespace std;
using namespace utils;

int main(int argc, char* argv[])
{
    utils::setupGPIO();
    if(argc < 2){
        cout << "Please provide arguments " << endl;
        return -1;
    }
    
    Compressor1 compressor(COMPRESSION_RATIO);
    EncoderRS<CODE_L, REDUNDANCY> encoder;

    int txrx = utils::readTxRx();
    std::cout << txrx << std::endl;
    if(string(argv[1]) == "tx") {
        
        SocketRadio socket(1);
        //SocketUDP socket(1,(char *)"localhost");
        StopWait protocol(&compressor, &encoder, &socket);
        
        char* file_name;
        if(argc > 2){
            file_name = argv[2];
        }
        else{
            file_name = (char *)"file.txt";
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
