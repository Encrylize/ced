#include <stdlib.h>

#include "alloc.h"


void *malloc_or_exit(size_t size) {
    void *ptr = malloc(size);

    if (ptr == NULL)
        exit(EXIT_FAILURE);

    return ptr;
}

void *realloc_or_exit(void *ptr, size_t new_size) {
    ptr = realloc(ptr, new_size);

    if (ptr == NULL)
        exit(EXIT_FAILURE);

    return ptr;
}
