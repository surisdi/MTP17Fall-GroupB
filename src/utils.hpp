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

#define COUT std::cout
//#define COUT NullStream()

typedef unsigned char byte;

namespace utils{

    // Protocol constants
	constexpr int CODE_L = 32;
	constexpr int REDUNDANCY = 2;
    extern const int DATA_L;
    extern const int PAYLOAD_L;
    extern const int PAYLOAD_L_GBN;
    extern const int LEN_ACK;
    extern const int WINDOW_SIZE;
    extern const int N_RETRANSMIT_ACK;

    //Compressor constants
    constexpr int MAX_CHUNK = 1000; 	// Used for preallocation
    constexpr int CHUNK_SIZE = 65536;
    constexpr int CHUNK_SIZE_BYTE = 2; //Size of last chunk has to be send!

    extern const int sizeCompressed;
    extern const int COMPRESSION_RATIO;

    //Ack constants
    extern const byte ack_sw;
    extern const byte ack_gbn;
    extern const byte nack;
    
    void bsc(unsigned char *sequence, int len, float prob);
    byte* read_text(const char *file_name, int *len);
    void printPacket(const byte *buffer, const int len, const int format);

    void printBits(size_t const size, void const * const ptr);

    // void open_led ...
}

#endif
