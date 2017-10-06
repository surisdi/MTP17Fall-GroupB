#ifndef UTILS_H_   /* Include guard */
#define UTILS_H_

#include <cstddef>


char* readFileBytes(const char *name, int *mylen);

void printBits(size_t const size, void const * const ptr);

void print_file(char *buffer, int len, int type);

#endif
