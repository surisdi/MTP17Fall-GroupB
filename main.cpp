#include "socket.h"
#include "compressor.h"
#include "encoder.h"
#include "protocol.h"

// main.cpp

int main(int argc, char* argv[])
{
    if(argc < 2){
        std::cout << "Please provide arguments " << file_name << std::endl;
        return -1;
    }

    int bufferLength = 100;
    Socket socket = SocketTCP(bufferLength);

    float compressionRate = 0.8;
    Compressor compressor = Compressor1(compressionRate);

    Encoder encoder = EncoderReedSolomon();

    Protocol protocol = ProtocolStopWait(socket, compressor, encoder);

    if(argv[1] == "tx"){
        const char* file_name = "";
        if(argc > 2)
            file_name = argv[2];
        else
            file_name = "file.txt";

        std::cout << "Input file name: " << file_name << std::endl;

        int sent_ok = protocol.send_text(file_name);

    }else{ // rx
        int received_ok = protocol.receive_text();
    }

    return 0;
}
