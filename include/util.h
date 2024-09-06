#ifndef UTIL_H
#define UTIL_H
#include <stdlib.h>


struct MemoryStruct {
    char *memory;
    size_t size;
};

size_t write_to_memory(void *contents, size_t size, size_t nmemb, void *userp);


void xor_encrypt_decrypt( char *data, size_t data_len, const char *key);


#endif // UTIL_H