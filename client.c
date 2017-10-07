#include "client.h"

#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>


int initiateSocket(char *ip, int port)
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = port;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        printf("ERROR opening socket");
    server = gethostbyname(ip);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        printf("ERROR connecting");


    printf("You are connected \n");
    return sockfd;
}


/*void radio (char *text, int len, int sockfd){

    char *buffer = (char *)malloc(sizeof(char) * len);
    int n = write(sockfd, text, strlen(text));
    if (n < 0)
         printf("ERROR writing to socket");

    bzero(buffer,256);
    n = read(sockfd,buffer,256);
    if (n < 0)
         printf("ERROR reading from socket");
    printf("%s\n",buffer);
}*/

int send (char *text, int len, int sockfd) {
	return write(sockfd, text, len);
}

int receive(char *buffer, int len, int sockfd) {
	return read(sockfd, buffer, len);
}


/*int main (){
	printf ("Hi\n");

	int sockfd = initiateSocket ((char *)"localhost", 5555);
	radio ((char *)"Hello", sockfd);
}*/
