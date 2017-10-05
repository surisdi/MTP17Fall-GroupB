#include <stdio.h>
#include <stdlib.h>

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
	for (i = 0; i < 5; i++) {
		printBits(sizeof(buffer[0]), &buffer[i]);
		//printf("%c", buffer[i]);
		//printf("%c", *(buffer + sizeof(buffer[0])*i));
	}
}  

int main() {
	int len;
	char *buffer = readFileBytes("file.txt", &len);

	print_file(buffer, len);
	printf("\n\n CANVI \n\n");

	//bsc(buffer, len, 0.0);

	print_file(&buffer[3], len);
}