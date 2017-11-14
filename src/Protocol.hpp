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
    
    int isAck(const char* r_ack);
    
    int createPacket(char *);

    std::mutex mtx;
    std::mutex mtx2;

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

    virtual void createMessage(char *message, char *buffer, int i, int len);

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
    void receiverQueueThread(std::atomic<bool>* last_packet, std::queue<char*> &q);
    void receiverProcessingThread(std::atomic<bool>* last_packet, std::queue<char*> &q);
    void receiveThread();

    bool timeoutExpired();

    virtual void createMessage(char *message, char *buffer, int i, int len);

    int id_send; // Next packet to be sent
    int id_base; // Expected packet to be acknowledged
};


#endif 
