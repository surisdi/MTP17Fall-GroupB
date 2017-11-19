//Protocol.hpp

#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include "Compressor.hpp"
#include "Socket.hpp"
#include "Encoder.hpp"

#include <mutex>
#include <queue>
#include <atomic>
#include <sys/time.h>

/***************** Base Class Protocol *****************/

class Protocol {

protected:
    Compressor *compressor;
    Encoder *encoder;
    Socket *socket;
    
    int isAck(const byte* r_ack);

    std::mutex mtx;
    std::mutex mtx2;
    
    bool lastPacket;
    std::queue<byte> q;

    bool finished_protocol;
    struct timespec clock_start, clock_now; 
    bool timer_running;

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

private:
    void receiveThread();

    virtual void createMessage(byte *message, byte *buffer, int i, int len);

    int flag_ack; // 1 ACK, 0 NOTHING RECEIVED, -1 NACK
    int flag_pac_num;
};


/***************** Derived Class Go Back N *****************/

class GoBackN: public Protocol {
public:
    GoBackN(Compressor *comp, Encoder *enc, Socket *sck);
    virtual int send_text(char *text) override;

    virtual int receive_text() override;

    virtual ~GoBackN();

private:
    void receiverQueueThread();
    void receiverProcessingThread();
    void receiveThread();

    bool timeoutExpired();

    void createMessage(byte *message, byte *buffer, int i, int len, bool isLast);

    bool parseMessage(byte *message, byte flags, byte previous, unsigned int *chunkSize, byte *dataSize, bool *lastPacket);

    int id_base; // Expected packet to be acknowledged
    int id_send; // Next packet to be sent
};


#endif 
