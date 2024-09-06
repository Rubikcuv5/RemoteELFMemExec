#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void xor_encrypt_decrypt(char *data, size_t data_len, const char *key) {
    size_t key_len = strlen(key);
    
    for (size_t i = 0; i < data_len; ++i) {
        data[i] ^= key[i % key_len];
    }
}

void encrypt_file(const char *file_path, const char *key) {
    FILE *input_file = fopen(file_path, "rb");
    if (input_file == NULL) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    // Get file size
    fseek(input_file, 0, SEEK_END);
    size_t file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    // Allocate memory for file content
    char *file_content = (char *)malloc(file_size);
    if (file_content == NULL) {
        perror("Error allocating memory");
        fclose(input_file);
        exit(EXIT_FAILURE);
    }

    // Read file content
    fread(file_content, 1, file_size, input_file);
    fclose(input_file);

    // Encrypt/Decrypt file content
    xor_encrypt_decrypt(file_content, file_size, key);

    // Generate output file name
    char output_file_path[256];
    snprintf(output_file_path, sizeof(output_file_path), "%s.enc", file_path);

    // Write encrypted content to output file
    FILE *output_file = fopen(output_file_path, "wb");
    if (output_file == NULL) {
        perror("Error opening output file");
        free(file_content);
        exit(EXIT_FAILURE);
    }

    fwrite(file_content, 1, file_size, output_file);
    fclose(output_file);

    // Free allocated memory
    free(file_content);

    printf("File encrypted successfully: %s\n", output_file_path);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file_path> <encryption_key>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *file_path = argv[1];
    const char *key = argv[2];

    encrypt_file(file_path, key);

    return EXIT_SUCCESS;
}


