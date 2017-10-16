// Protocol.hpp

#include "Encoder.hpp"
#include "Socket.hpp"
#include "Compressor.hpp"

#ifndef PROTOCOL_H
#define PROTOCOL_H

/**********************************************************************/

// Base class
class Protocol {
	public:
		Protocol(Socket *soc, Compressor *comp, Encoder *enc){
			socket = soc;
			compressor = comp;
			encoder = enc;
		}
		// pure virtual function
		virtual int send_text(char* file_name) = 0;
		virtual int receive_text() = 0;

	protected:
		Socket *socket;
		Compressor *compressor;
		Encoder *encoder;
};

/*********************************************************************/

// Stop & Wait Protocol
class ProtocolStopWait: public Protocol{
	public:
		ProtocolStopWait(Socket *soc, Compressor *comp, Encoder *enc);
		int send_text(char* file_name) override;
		int receive_text() override;

};

/************************************************************************/

// Another protocol

#endif