# Encryptor

A utility for encrypting files using XOR encryption with a provided encryption key.

## Compilation

To compile the `encrypt_file.c` source file, use the following command:

```bash
gcc -o encrypt_file encrypt_file.c
```


## Usage

To encrypt a file, run the following command:

```bash
./encrypt_file <file_path> <encryption_key>
```

- `<file_path>`: The path to the file you want to encrypt.
- `<encryption_key>`: The encryption key used for XOR encryption.

## Example

```bash
./encrypt_file test_implant 0331afffba74a654b4e8
```

