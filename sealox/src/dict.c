#include "dict.h"
#include "memory.h"

// null key and not tombstone
#define IS_EMPTY_SLOT(target) target->key == NULL && !IS_BOOL(target->val)

DictEntry* dict_find_entry(Dict* dict, ObjStr* key) {
    int cap = dict->capacity;
    if (cap == 0) {
        return NULL;
    }

    int x = key->hash;
    int i_target = (key->hash) % cap;
    for (int i = 0; i < cap; i++) {
        DictEntry* target = &dict->entries[i_target];

        /*
         * We can stop the probing sequence when we hit an empty slot,
         * because conflicts result in a sequence of occupied slots
         * (possibly tombstones).
         */
        if (IS_EMPTY_SLOT(target)) {
            return NULL;
        }

        // TODO: compare by value
        if (target->key == key) {
            return target;
        }

        i_target = (i_target + 1) % cap;
    }
    return NULL;
}

DictEntry* dict_find_empty_slot(Dict* dict, ObjStr* key) {
    int cap = dict->capacity;
    int i_target = (key->hash) % cap;
    for (int i = 0; i < cap; i++) {
        DictEntry* target = &dict->entries[i_target];

        if (IS_EMPTY_SLOT(target)) {
            return target;
        }

        i_target = (i_target + 1) % cap;
    }
    return NULL;
}

void dict_grow(Dict* dict) {
    int cap = CALC_CAP(dict->capacity);
    DictEntry* new_entries = REALLOC_ARR(DictEntry, dict->entries, cap);
    for (int i = 0; i < cap; i++) {
        new_entries[i].key = NULL;
        new_entries[i].val = MK_NIL_VAL;
    }

    dict->count = 0;
    for (int i = 0; i < dict->capacity; i++) {
        DictEntry* entry = &dict->entries[i];
        if (entry->key == NULL) {
            continue;
        }

        DictEntry* dest = dict_find_entry(dict, entry->key);
        dest->key = entry->key;
        dest->val = entry->val;
        dict->count++;
    }

    free(dict->entries);
    dict->entries = new_entries;
    dict->capacity = cap;
}

void dict_init(Dict* dict) {
    dict->count = 0;
    dict->capacity = 0;
    dict->entries = NULL;
}

void dict_free(Dict* dict) {
    free(dict->entries);
    dict_init(dict);
}

bool dict_get(Dict* dict, ObjStr* key, Val* val) {
    DictEntry* match = dict_find_entry(dict, key);

    if (match == NULL) {
        return false;
    }

    *val = match->val;
    return true;
}

bool dict_put(Dict* dict, ObjStr* key, Val val) {
    if (dict->count + 1 > dict->capacity) {
        dict_grow(dict);
    }

    DictEntry* match = dict_find_empty_slot(dict, key);
    if (match == NULL) {
        return false;
    }

    match->key = key;
    match->val = val;
    dict->count++;

    return true;
}

bool dict_del(Dict* dict, ObjStr* key) {
    DictEntry* match = dict_find_entry(dict, key);

    if (match == NULL) {
        return false;
    }

    // create tombstone
    match->key = NULL;
    match->val = MK_BOOL_VAL(true);

    return true;
}
