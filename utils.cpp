// utils.cpp

#include "utils.hpp"

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

	int read_text(std::string file_name, char* buffer){
		int len;
    	buffer = readFileBytes(file_name, &len);
    	return len;
	}
}