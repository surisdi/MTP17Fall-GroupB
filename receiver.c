#include <cstdio>
#include <cstdlib>

#include "comms.h"
#include "utils.h"
#include "server.h"

#define TCP_PORT 5005
#define REDUNDANCY 30
#define PAYLOAD 10


int main()
{
	char packet[PAYLOAD];
	int socket = initiateServer(TCP_PORT);

	FILE *outputFile = fopen("output.txt", "w+b");

	int res;
	int n = 1;
	while (true) {
		n = readFromSocket(packet, PAYLOAD, socket);

        if (n == 0)
            break;

        printf(packet);
        res = fputs(packet, outputFile);
	}

	fclose(outputFile);




}
