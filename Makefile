# Nombre del compilador
CC = gcc

# Flags del compilador (modo debug por defecto)
CFLAGS = -Iinclude -lcurl   -DDEBUG   -Wall 

# Flags del compilador para la compilación estática (sin debug)
STATIC_CFLAGS = -Iinclude -static -lcurl 


# Flags del compilador para sistemas legacy (modo debug por defecto)
CFLAGS_LEGACY = -Iinclude -lcurl  -DDEBUG -Wall -std=c99  -D_POSIX_C_SOURCE=200112L -DLEGACY_COMPILATION

# Flags del compilador para la compilación estática en sistemas legacy (sin debug)
STATIC_CFLAGS_LEGACY = -Iinclude -static -lcurl  -std=c99  -lpthread -lrt -D_POSIX_C_SOURCE=200112L -DLEGACY_COMPILATION



# Nombre del archivo ejecutable
TARGET = elfMemExecutor

# Nombre del archivo ejecutable estático
STATIC_TARGET = elfMemExecutor

# Directorio de origen
SRC_DIR = src

# Archivos fuente
SRC = $(wildcard $(SRC_DIR)/*.c)

# Reglas por defecto
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(SRC)
	$(CC) -o $@ $(SRC) $(CFLAGS)

# Regla para compilar el ejecutable estático
static: $(SRC)
	$(CC) -o $(STATIC_TARGET) $(SRC) $(STATIC_CFLAGS)

# Regla para limpiar archivos compilados
clean:
	rm -f $(TARGET) $(STATIC_TARGET)

# Regla para compilar en sistemas legacy
legacy: $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS_LEGACY)

# Regla para compilar el ejecutable estático en sistemas legacy
legacy_static: $(SRC)
	$(CC) -o $(STATIC_TARGET) $(SRC) $(STATIC_CFLAGS_LEGACY)



# Regla para reconstruir
rebuild: clean all

.PHONY: all clean rebuild static
