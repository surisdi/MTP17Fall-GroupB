#include "SocketTCP.h"


int main (){

	SocketTCP sock = SocketTCP ((char*) "localhost", 5000, false, 20);


	sock.writeBuff ((char*) "Hello biatches");

	//int sockfd = initiateSocket ((char *)"localhost", 5555);
	//radio ((char *)"Hello", sockfd);
}