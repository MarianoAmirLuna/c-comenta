
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <../include/bitarray_mmap.h>
#include <commons/bitarray.h>
#include <sys/mman.h>


int bitarray_write_to_file(t_bitarray *bitarray, const char *filename) {
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    // Asegurarse de que el archivo tenga el tamanio adecuado
    if (ftruncate(fd, bitarray->size) == -1) {
        perror("ftruncate");
        close(fd);
        return -1;
    }

    // Escribir el bitarray en el archivo
    if (write(fd, bitarray->bitarray, bitarray->size) != bitarray->size) {
        perror("write");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

t_bitarray* bitarray_mmap_from_file(const char *filename, bit_numbering_t mode) {
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("open");
        return NULL;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return NULL;
    }

    char *mapped = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return NULL;
    }

    close(fd);

    return bitarray_create_with_mode(mapped, st.st_size, mode);
}

void bitarray_munmap(t_bitarray *bitarray) {
    if (msync(bitarray->bitarray, bitarray->size, MS_SYNC) == -1) {
        perror("msync");
    }

    if (munmap(bitarray->bitarray, bitarray->size) == -1) {
        perror("munmap");
    }

    bitarray_destroy(bitarray);
}

