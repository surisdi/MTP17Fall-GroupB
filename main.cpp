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
    const int CODE_L = 32;
    const int REDUNDANCY = 2;

    if(argc < 2){
        cout << "Please provide arguments " << endl;
        return -1;
    }
    
    //TODO: Aixo no fa res, s'ha d'eliminar, només es una clase de prints
    Compressor1 compressor(0.5, 5);
    EncoderRS<CODE_L, REDUNDANCY> encoder;
    
    if(string(argv[1]) == "tx") {
        
        SocketRadio socket(1);
        StopWait protocol(&compressor, &encoder, &socket);
        
        char* file_name;
        if(argc > 2){
            file_name = argv[2];
        }
        else{
            file_name = (char *)"file.txt";
        }
        
        std::cout << "Input file name: " << file_name << std::endl;
        
        int sent_ok = protocol.send_text(file_name);
        
    } else {
        
        SocketRadio socket(0);
        StopWait protocol(&compressor, &encoder, &socket);
        
        int received_ok = protocol.receive_text();
    }
    
    return 0;
}
