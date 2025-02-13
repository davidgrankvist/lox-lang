#ifndef dict_h
#define dict_h

#include "common.h"
#include "ops.h"

typedef struct {
    ObjStr* key;
    Val val;
} DictEntry;

typedef struct {
    int count;
    int capacity;
    DictEntry* entries;
} Dict;

void dict_init(Dict* dict);
void dict_free(Dict* dict);
bool dict_get(Dict* dict, ObjStr* key, Val* val);
bool dict_put(Dict* dict, ObjStr* key, Val val);
bool dict_del(Dict* dict, ObjStr* key);

/*
 * Look by up a string key by its value. This is to support string interning.
 */
ObjStr* dict_get_str(Dict* dict, const char* start, int length, uint32_t hash);

#endif
