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

struct Chunk {
	unsigned char *data;
	unsigned long *len;
};

//#define COUT std::cout
#define COUT NullStream()

namespace utils{

    // Protocol constants
//    extern const int CODE_L;
//    extern const int REDUNDANCY;
	constexpr int CODE_L = 32;
	constexpr int REDUNDANCY = 2;
    extern const int DATA_L;
    extern const int PAYLOAD_L;
    extern const int PAYLOAD_L_GBN;
    extern const int LEN_ACK;
    extern const int WINDOW_SIZE;

    //Compressor constants
    constexpr int MAX_CHUNK = 1000;
    extern const int CHUNK_SIZE;
    extern const int sizeCompressed;
    extern const int COMPRESSION_RATIO;

    extern const char ack_sw;
    extern const char ack_gbn;
    extern const char nack;
    
    void bsc(char *sequence, int len, float prob);
    char* read_text(const char *file_name, int *len);
    void printPacket(const char *buffer, const int len, const int format);

    void printBits(size_t const size, void const * const ptr);

    
    // void open_led ...

}

#endif
