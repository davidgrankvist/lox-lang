#ifndef memory_h
#define memory_h

#include "common.h"
#include "stdlib.h"
#include "ops.h"

#define DEFAULT_CAP 8
#define CALC_CAP(cap) \
    cap < DEFAULT_CAP ? DEFAULT_CAP : cap * 2;

#define REALLOC_ARR(type, ptr, new_cap) \
    (type*)realloc_arr(ptr, sizeof(type) * new_cap)

void* realloc_arr(void* ptr, size_t new_cap);

ObjStr* alloc_str(char* start, int length);
ObjStr* cp_str(const char* start, int length);

void free_objects();

#endif
