#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "debug.h"



size_t write_to_memory(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + real_size + 1);
    if(ptr == NULL) {
        DEBUG_PRINT("Not enough memory to realloc\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, real_size);
    mem->size += real_size;
    mem->memory[mem->size] = 0;

    return real_size;
}

void xor_encrypt_decrypt( char *data, size_t data_len, const char *key) {
    size_t key_len = strlen(key);
    
    for (size_t i = 0; i < data_len; ++i) {
        data[i] ^= key[i % key_len];
    }
}

