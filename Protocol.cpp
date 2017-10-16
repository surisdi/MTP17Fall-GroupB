// Protocol.cpp

#include "Protocol.hpp"
#include "utils.hpp"

/*********************************************************************/

// Stop & Wait Protocol
ProtocolStopWait::ProtocolStopWait(Socket *soc, Compressor *comp, Encoder *enc)
:Protocol(soc, comp, enc){}

int ProtocolStopWait::send_text(char* file_name){
	// TODO
	return 0;
}

int ProtocolStopWait::receive_text(){
	// TODO
	return 0;
}

/************************************************************************/

// Another protocol