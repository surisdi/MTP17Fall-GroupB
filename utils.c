#include "utils.h"

#include <cstdio>
#include <cstdlib>

char* readFileBytes(const char *name, int *mylen)  
{  
    FILE *fl = fopen(name, "r");  
    fseek(fl, 0, SEEK_END);  
    long len = ftell(fl);  
    char *ret = (char *)malloc(sizeof(char)*len);   
    fseek(fl, 0, SEEK_SET);  
    fread(ret, 1, len, fl);  
    fclose(fl);

    *mylen = len;
    return ret;  
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

void print_file(char *buffer, int len, int type)
{
    int i;
    for (i = 0; i < len; i++) {
        if (type == 2)
            printBits(sizeof(buffer[0]), &buffer[i]);
        else if(type == 0)
            printf(" %u ", buffer[i]);
        else
            printf("%c", buffer[i]);
    }
}
