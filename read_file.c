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

int main() {
	int len;
	char *buffer = readFileBytes("file.txt", &len);

	int i;
	for (i = 0; i < len; i++) {
		printf("%c", buffer[i]);
	}
	printf("\n");
}