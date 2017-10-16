#include "Socket.hpp"
#include "Compressor.hpp"
#include "Encoder.hpp"
#include "Protocol.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <unistd.h>

// main.cpp

int main(int argc, char* argv[])
{
    if(argc < 2){
        std::cout << "Please provide arguments " << std::endl;
        return -1;
    }

    float compressionRate = 0.8;
    Compressor *compressor = new Compressor1(compressionRate);

    Encoder *encoder = new EncoderReedSolomon();

    Protocol *protocol;
    Socket *socket;

    if(argv[1] == "tx"){

        int bufferSize = 100;
        bool isServer = true;
        char* ip = (char*)"localhost";

        socket = new SocketTCP(ip, isServer, bufferSize);

        protocol = new ProtocolStopWait(socket, compressor, encoder);

        char* file_name = (char*)"";
        if(argc > 2){
            file_name = argv[2];
        }
        else{
            file_name = (char*)"file.txt";
        }

        std::cout << "Input file name: " << file_name << std::endl;

        int sent_ok = protocol->send_text(file_name);

    }else{ // rx

        int bufferSize = 100;
        bool isServer = false;
        char* ip = (char*)"localhost";
        Socket *socket = new SocketTCP(ip, isServer, bufferSize);

        Protocol *protocol = new ProtocolStopWait(socket, compressor, encoder);


        int received_ok = protocol->receive_text();
    }

    delete[] compressor;
    delete[] encoder;
    delete[] protocol;
    delete[] socket;

    return 0;
}
