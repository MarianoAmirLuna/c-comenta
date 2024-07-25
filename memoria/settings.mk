# Libraries
LIBS=utils commons pthread readline m

# Custom libraries' paths
STATIC_LIBPATHS=../utils

# Include paths
INCLUDE_PATHS=-I../include -I./utils

# Compiler flags
CDEBUG=-g -Wall -DDEBUG -fdiagnostics-color=always
CRELEASE=-O3 -Wall -DNDEBUG -fcommon

# Arguments when executing with start, memcheck or helgrind
ARGS=

# Valgrind flags
MEMCHECK_FLAGS=--track-origins=yes
HELGRIND_FLAGS=

# Regla para compilar en modo depuración
debug: CFLAGS=$(CDEBUG)
debug: $(EXEC)

# Regla para compilar en modo release
release: CFLAGS=$(CRELEASE)
release: $(EXEC)

# Regla para enlazar el ejecutable
$(EXEC): $(OBJS)
	$(LD) $(CFLAGS) -o $@ $^ $(addprefix -l, $(LIBS)) $(addprefix -L, $(STATIC_LIBPATHS))

# Regla para compilar archivos fuente en objetos
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Regla para limpiar los archivos compilados
clean:
	-rm -rfv obj bin
	rm -f $(OBJS) $(EXEC)