// utils.cpp

#include "utils.hpp"
#include <cstdio>
#include <cstdlib> // for the random
#include <cstdint>
#include <iostream>
#include <bitset>

namespace utils{
    
    extern const byte ack_sw = 0xFC;
    extern const byte ack_gbn = 0xE0;
    extern const byte nack = 0x00;
    
    // Protocol constants
    extern const int DATA_L = CODE_L - REDUNDANCY;
    extern const int PAYLOAD_L = DATA_L - 1;
    extern const int PAYLOAD_L_GBN = DATA_L - 2; // 2 bytes of flags
    extern const int LEN_ACK = 1;
    extern const int WINDOW_SIZE = 3; // 5 bits for the packet ID
    extern const int N_RETRANSMIT_ACK = 10;
    
    // Compression constants
    extern const int sizeCompressed = (CHUNK_SIZE*1.1) + 12;
    extern const int COMPRESSION_RATIO = 9;

    void bsc(unsigned char *sequence, int len, float prob_error)
    {	/*
        double w;
        int b;
        int coin;
        
        for (int i=0; i < len; i++) {
            for (b = 0; b < 8; b++) {
                w = (double)rand()/ (double) RAND_MAX;
                coin = w < prob_error;
                sequence[i] = sequence[i] ^ (coin << b);
            }
        }*/
    }

    void printPacket(const byte *buffer, const int len, const int format) {
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
    }
        
    
    byte* read_text(const char *file_name, int* mylen){
        
        FILE *fl = fopen(file_name, "rb");
        fseek(fl, 0, SEEK_END);  
        long len = ftell(fl);  
        byte *buffer = (byte *)malloc(sizeof(byte)*len);
        fseek(fl, 0, SEEK_SET);  
        fread(buffer, 1, len, fl);  
        fclose(fl);
        
        *mylen = len;
        
        return buffer;
    }

    bool setupGPIO(){
        if (!bcm2835_init())
            return 0;
        // Configure LEDs as output
        bcm2835_gpio_fsel(RED_LED, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_fsel(GREEN_LED, BCM2835_GPIO_FSEL_OUTP);
        // Configure Tx Rx Switch as input and pullup
        bcm2835_gpio_fsel(TXRX_PORT, BCM2835_GPIO_FSEL_INPT);
        bcm2835_gpio_set_pud(TXRX_PORT, BCM2835_GPIO_PUD_UP);
        // Configure Network Switch as input and pullup
        bcm2835_gpio_fsel(NET_PORT, BCM2835_GPIO_FSEL_INPT);
        bcm2835_gpio_set_pud(NET_PORT, BCM2835_GPIO_PUD_UP);
        return 1;
    }

    bool destroyGPIO(){
        return bcm2835_close();
    }

    uint8_t readTxRx(){
        return bcm2835_gpio_lev(TXRX_PORT);
    }

    uint8_t readNetworkMode(){
        return bcm2835_gpio_lev(NET_PORT);
    }

    void blinkRed(){
        bcm2835_gpio_write(RED_LED, HIGH);
        bcm2835_delay(10);
        bcm2835_gpio_write(RED_LED, LOW);
    }

    void blinkGreen(){
        bcm2835_gpio_write(GREEN_LED, HIGH);
        bcm2835_delay(10);
        bcm2835_gpio_write(GREEN_LED, LOW);
    }
}
