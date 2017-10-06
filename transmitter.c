#include <cstdio>
#include <cstdlib>

#include "comms.h"
#include "utils.h"

#define TCP_PORT 5005
#define REDUNDANCY 30
#define PAYLOAD 10


int main() {
    int len;
    char *buffer = readFileBytes("file.txt", &len);
    len = 40;

    //print_file(buffer, len, 0);
    //printf("\n\n\n\n");

    //bsc(buffer, len, 0.1);

    //print_file(buffer, len, 0);

    //Flags to avoid errors 
    int s_last = 0, r_next = 0;  

    int i = 0;


    char *data;
    char *packet = (char *)malloc(sizeof(char) * (len + 1));
    char ack[5];

    int pay_len;

    int extra = ((len % PAYLOAD) != 0);
    while (i < len / PAYLOAD + extra) {

        data = &buffer[i * PAYLOAD];
        if ( (i + 1) * PAYLOAD > len ) { //Last packet may be smaller
            pay_len = len - ( i * PAYLOAD );
        } else {
            pay_len = PAYLOAD;
        }


        packet_builder(packet, data, pay_len, s_last);
        //radio.write(packet);

        print_file(packet, pay_len + 1, 0);
        printf("\n\n");

        i++;

        /*
        //radio.read(ack);
        r_next = get_ack_label(ack);

        if (isAck(ack)) {   //If it is NACK resend the same packet
            if (r_next != s_last) { //Move to the next packet
                s_last = s_last ^ 1;
                i++;
            }
        }*/
    }

}
