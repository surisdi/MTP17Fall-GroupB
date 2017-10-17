//Protocol.hpp

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "Compressor.hpp"
#include "Socket.hpp"
#include "Encoder.hpp"

/***************** Base Class Protocol *****************/

class Protocol {

protected:
	Compressor *compressor;
	Encoder *encoder;
	Socket *socket;

public:
	Protocol(Compressor *, Encoder *, Socket *);

	virtual int send_text(const char*) = 0;
	
	virtual int receive_text() = 0;

	virtual ~Protocol();
};


/***************** Derived Class StopWait *****************/

class StopWait: public Protocol {
public:
	StopWait(Compressor *comp, Encoder *enc, Socket *sck);

	virtual int send_text(const char *text) override;
	
	virtual int receive_text() override;
	
	virtual ~StopWait();
};


#endif /* PROTOCOL_H_ */