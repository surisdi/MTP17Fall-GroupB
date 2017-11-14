// main.cpp

#include <iostream>

#include "Protocol.hpp"
#include "Encoder.hpp"
#include "Compressor.hpp"
#include "Socket.hpp"

#include "utils.hpp"

using namespace std;


int main(int argc, char* argv[])
{
    if(argc < 2){
        cout << "Please provide arguments " << endl;
        return -1;
    }
    
    Compressor1 compressor(utils::COMPRESSION_RATIO);
    EncoderRS<utils::CODE_L, utils::REDUNDANCY> encoder;

    if(string(argv[1]) == "tx") {
        
        SocketRadio socket(1);
	//    SocketUDP socket(1,(char*)"localhost");
        GoBackN protocol(&compressor, &encoder, &socket);
        
        char* file_name;
        if(argc > 2){
            file_name = argv[2];
        }
        else{
            file_name = (char *)"file.txt";
        }
        
        cout << "Input file name: " << file_name << endl;
        
        int sent_ok = protocol.send_text(file_name);
        
    } else {
        
        SocketRadio socket(0);
	//    SocketUDP socket(0,(char*)"localhost");
        GoBackN protocol(&compressor, &encoder, &socket);
        
        int received_ok = protocol.receive_text();
    }
    
    return 0;
}
