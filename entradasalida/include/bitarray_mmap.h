#ifndef BITARRAY_MMAP_H_
#define BITARRAY_MMAP_H_

#include <commons/bitarray.h>

int bitarray_write_to_file(t_bitarray *bitarray, const char *filename);
t_bitarray* bitarray_mmap_from_file(const char *filename, bit_numbering_t mode);
void bitarray_munmap(t_bitarray *bitarray);

#endif /* BITARRAY_MMAP_H_ */
