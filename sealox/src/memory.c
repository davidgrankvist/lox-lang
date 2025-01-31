#include "memory.h"

void* realloc_arr(void* ptr, size_t new_cap) {
    if (new_cap == 0) {
        free(ptr);
        return NULL;
    }
    void* new_ptr = realloc(ptr, new_cap);
    if (new_ptr == NULL) {
        exit(1);
    }
    return new_ptr;
}
