#include "SocketTCP.h"
#include <stdio.h>


int main (){

	SocketTCP sock = SocketTCP ((char*) "localhost", 5000, true, 20);

	char* buff = new char [1024];
	sock.read_blocking (buff);

	printf ("%s\n", buff);

	//int sockfd = initiateSocket ((char *)"localhost", 5555);
	//radio ((char *)"Hello", sockfd);
}