#ifndef test_common_h
#define test_common_h

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define ASSERT(b, msg) \
    do { \
        if (!(b)) { \
            fprintf(stderr, "FAIL: \"%s\" at line %d in %s\n", msg, __LINE__, __FILE__); \
            exit(1); \
        } \
    } while(false)

#define BEGIN_TEST() printf("RUNNING %s\n", __func__)
#define END_TEST() printf("PASS %s\n", __func__);
#define BEGIN_SUITE() printf("=== %s ===\n", __FILE__)
#define END_SUITE() printf("\n")

#endif
