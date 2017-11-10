// utils.hpp

#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdio>
#include <cstdlib>

class NullStream {
    public:
    NullStream() { }
    template<typename T> NullStream& operator<<(T const&) { return *this; }
};

    #define COUT std::cout
    //#define COUT NullStream()

    namespace utils{

    extern const int CODE_L;
    extern const int REDUNDANCY;
    extern const int DATA_L;
    extern const int PAYLOAD_L;
    extern const int PAYLOAD_L_GBN;
    extern const int LEN_ACK;
    extern const int WINDOW_SIZE;


    extern const char ack;
    extern const char nack;
    
    void bsc(char *sequence, int len, float prob);
    char* read_text(char* file_name, int* len);
    void printPacket(const char *buffer, const int len, const bool isHex);

    void printBits(size_t const size, void const * const ptr);

    
    // void open_led ...

}

#endif