// Protocol.hpp

#ifndef PROTOCOL_H
#define PROTOCOL_H

/**********************************************************************/

// Base class
class Protocol {
	public:
		Protocol(Socket soc, Compressor comp, Encoder enc){
			socket = soc;
			compressor = comp;
			encoder = enc;
		}
		// pure virtual function
		virtual int send_text() = 0;
		virtual int receive_text() = 0;

	private:
		Socket socket;
		Compressor compressor;
		Encoder encoder;
}

/*********************************************************************/

// Stop & Wait Protocol
class ProtocolStopWait: public Protocol{

	ProtocolStopWait();
	void send_text();
	void receive_text();

/************************************************************************/

// Another protocol

#endif