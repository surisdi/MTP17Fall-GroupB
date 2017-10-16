// utils.cpp

#include "utils.hpp"
#include <cstdio>
#include <cstdlib> // for the random

namespace utils{

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

	char* read_text(char* file_name, int* mylen){

	    FILE *fl = fopen(file_name, "rb");  
	    fseek(fl, 0, SEEK_END);  
	    long len = ftell(fl);  
	    char *buffer = (char *)malloc(sizeof(char)*len);   
	    fseek(fl, 0, SEEK_SET);  
	    fread(buffer, 1, len, fl);  
	    fclose(fl);

	    *mylen = len;

    	return buffer;
	}
}