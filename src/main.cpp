// main.cpp

#include <iostream>

#include "Protocol.hpp"
#include "Encoder.hpp"
#include "Compressor.hpp"
#include "Socket.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	if(argc < 2){
		cout << "Please provide arguments " << endl;
		return -1;
	}

	Compressor1 compressor(0.8, 5);
	EncoderDummy encoder;

	if(string(argv[1]) == "tx") {

		int bufferSize = 100;
		bool isServer = false;
		char ip[] = "localhost";

		SocketRadio socket;

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

	}else { // rx

		int bufferSize = 100;
		bool isServer = true;
		char ip[] = "localhost";

		SocketTCP socket(ip, isServer);
		StopWait protocol(&compressor, &encoder, &socket);

		int received_ok = protocol.receive_text();
	}

	return 0;
}
