#include <stdio.h>
#include <stdlib.h>

#define TCP_PORT = 5005
#define REDUNDANCY = 30
#define PAYLOAD = 40

char* readFileBytes(const char *name, int *mylen)  
{  
    FILE *fl = fopen(name, "r");  
    fseek(fl, 0, SEEK_END);  
    long len = ftell(fl);  
    char *ret = malloc(len);  
    fseek(fl, 0, SEEK_SET);  
    fread(ret, 1, len, fl);  
    fclose(fl);

    *mylen = len;
    return ret;  
}

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

void printBits(size_t const size, void const * const ptr)
{
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

void print_file(char *buffer, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		printBits(sizeof(buffer[0]), &buffer[i]);
	}
}

//TO-DO It builds the complete packet to be send (Adds Codes...)
char* packet_builder(char *data, int len) {}

// TO-DO 
bool isAck(char *ack) {}

//TO-DO It extracts the label from the ACK
bool get_ack_label(char *ack) {}

int main() {
	int len;
	char *buffer = readFileBytes("file.txt", &len);

	print_file(buffer, len);
	printf("\n\n\n\n");

	bsc(buffer, len, 0.1);

	print_file(buffer, len);

    bool s_last = 0; r_next = 0; #Flags to avoid errors
    int i = 0;


    char *data, *packet, *ack;
    int pay_len;

    while (i < len / PAYLOAD + 1) {

        data = &buffer[i * PAYLOAD];
        if ( (i + 1) * PAYLOAD > len ) { //Last packet may be smaller
            pay_len = len - ( i * PAYLOAD );
        } else {
            pay_len = PAYLOAD;
        }

        packet = packet_builder(data, pay_len);
        //radio.write(packet);

        //radio.read(ack);
        r_next = get_ack_label(ack);

        if (isAck(ack)) {	//If it is NACK resend the same packet
            if (r_next != s_last) { //Move to the next packet
                s_last = s_last ^ 1;
                i++;
            }
        }

    }

}