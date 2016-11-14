#ifndef _ALLOC_H
#define _ALLOC_H

#include <stdlib.h>


void *malloc_or_exit(size_t size);
void *realloc_or_exit(void *ptr, size_t new_size);


#endif
