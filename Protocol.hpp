//Protocol.hpp

#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include "Compressor.hpp"
#include "Socket.hpp"
#include "Encoder.hpp"

/***************** Base Class Protocol *****************/

class Protocol {

protected:
    Compressor *compressor;
    Encoder *encoder;
    Socket *socket;
    
    int isAck(const char* r_ack);
    
    int createPacket(char *);

public:
    Protocol(Compressor *, Encoder *, Socket *);

    virtual int send_text(char*) = 0;

    virtual int receive_text() = 0;

    virtual ~Protocol();
};


/***************** Derived Class StopWait *****************/

class StopWait: public Protocol {
public:
    StopWait(Compressor *comp, Encoder *enc, Socket *sck);

    virtual int send_text(char *text) override;

    virtual int receive_text() override;

    virtual ~StopWait();
};


#endif 
