// utils.cpp

#include "utils.hpp"
#include <cstdio>
#include <cstdlib> // for the random
#include <iostream>

namespace utils{
    
    extern const char ack = 0xFC;
    extern const char nack = 0x00;
    
    extern const int CODE_L = 32;
    extern const int REDUNDANCY = 2;
    extern const int DATA_L = CODE_L - REDUNDANCY;
    extern const int PAYLOAD_L = DATA_L - 1;
    extern const int LEN_ACK = 1;
    
    void bsc(char *sequence, int len, float prob)
    {
        double w;
        int b;
        int coin;
        
        for (int i=0; i < len; i++) {
            for (b = 0; b < 8; b++) {
                w = (double)rand()/ (double) RAND_MAX;
                coin = w < prob;
                sequence[i] = sequence[i] ^  (coin << b);
            }
        }
    }
    
    void printBits(size_t const size, void const * const ptr){
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

    void printPacket(const char *buffer, const int len, const bool isHex) {
        
        std::cout << "[ ";
        for (int i = 0; i < len; i++) {
            if(isHex)
                std::cout << std::hex << buffer[i];
            else
                std::cout << buffer[i];
        }
        std::cout << " ]" << std::endl;
    }
                
        
    
    char* read_text(char* file_name, int* mylen){
        
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
