// utils.cpp

#include "utils.hpp"
#include <cstdio>
#include <cstdlib> // for the random
#include <iostream>
#include <bitset>

namespace utils{
    
    extern const char ack_sw = 0xFC;
    extern const char ack_gbn = 0xE0;
    extern const char nack = 0x00;
    
    // Protocol constants
    extern const int DATA_L = CODE_L - REDUNDANCY;
    extern const int PAYLOAD_L = DATA_L - 1;
    extern const int PAYLOAD_L_GBN = DATA_L - 2; // 2 bytes of flags
    extern const int LEN_ACK = 1;
    extern const int WINDOW_SIZE = 31; // 5 bits for the packet ID
    
    // Compression constants
    extern const int sizeCompressed = (CHUNK_SIZE*1.1) + 12;
    extern const int COMPRESSION_RATIO = 9;

    void bsc(char *sequence, int len, float prob_error)
    {
        double w;
        int b;
        int coin;
        
        for (int i=0; i < len; i++) {;
            for (b = 0; b < 8; b++) {
                w = (double)rand()/ (double) RAND_MAX;
                coin = w < prob_error;
                sequence[i] = sequence[i] ^ (coin << b);
            }
        }
    }
    
    void printBits(size_t const size, void const * const ptr){}
    void printPacket(const char *buffer, const int len, const int format) {}

    /*void printBits(size_t const size, void const * const ptr){
        unsigned char *b = (unsigned char*) ptr;
        unsigned char byte;
        int i, j;

        for (i=size-1;i>=0;i--)
        {
            for (j=7;j>=0;j--)
            {
                byte = (b[i] >> j) & 1;
                printf("%u", byte);
            }
        }
        puts("");
    }

    void printPacket(const char *buffer, const int len, const int format) {
        // format: 0 char, 1 hex, 2 decimal
        COUT<< "[ ";
        for (int i = 0; i < len; i++) {
            switch(format){
                case 0:
                    COUT<< buffer[i];
                    break;
                case 1: 
                    COUT<< std::hex << static_cast<unsigned char>(buffer[i]) << " ";
                    break;
                case 2:
                	printf(" %u ", (unsigned char)buffer[i]);
                    //COUT<< std::dec << static_cast<unsigned char>(buffer[i]) << " ";
                    break;
            }
        }

        COUT<< " ]\n";
    }*/
        
    
    char* read_text(const char *file_name, int* mylen){
        
        FILE *fl = fopen(file_name, "rb");  
        fseek(fl, 0, SEEK_END);  
        long len = ftell(fl);  
        char *buffer = (char *)malloc(sizeof(char)*len);   
        fseek(fl, 0, SEEK_SET);  
        fread(buffer, 1, len, fl);  
        fclose(fl);
        
        *mylen = len;
        
        return buffer;
    }
}
