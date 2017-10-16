// Protocol.cpp

#include "Protocol.hpp"
#include "Compressor.hpp"
#include "Encoder.hpp"
#include "Socket.hpp"
#include "utils.hpp"

/*********************************************************************/

// Stop & Wait Protocol
ProtocolStopWait::ProtocolStopWait(Socket soc, Compressor comp, Encoder enc)
: Protocol(soc, comp, enc){
	# TODO
}

void send_text(){
	# TODO
}

void receive_text(){
	# TODO
}

/************************************************************************/

// Another protocol