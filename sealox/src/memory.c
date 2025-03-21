#include <string.h>
#include "memory.h"
#include "vm.h"
#include "dict.h"

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

Obj* allocate_obj (size_t size, ObjType type, bool with_gc) {
    Obj* obj = (Obj*)realloc_arr(NULL, size);
    obj->type = type;

    if (with_gc) {
        // append to VM state for garbage collection
        obj->next = vm.objects;
        vm.objects = obj;
    }

    return obj;
}

#define ALLOCATE_OBJ(type, otype) \
    (type*)allocate_obj(sizeof(type), otype, true)

#define ALLOCATE_OBJ_NO_GC(type, otype) \
    (type*)allocate_obj(sizeof(type), otype, false)

static uint32_t calc_str_hash(const char* start, int length) {
    // FNV-1a
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)start[i];
        hash *= 16777619;
    }
    return hash;
}

ObjStr* alloc_str(char* start, int length, uint32_t hash) {
    ObjStr* str = ALLOCATE_OBJ(ObjStr, OBJ_STR);
    str->length = length;
    str->chars = start;
    str->hash = hash;

    // store for deduplication
    dict_put(&vm.strings, str, MK_NIL_VAL);

    return str;
}

ObjStr* take_str(char* start, int length) {
    uint32_t hash = calc_str_hash(start, length);
    ObjStr* interned = dict_get_str(&vm.strings, start, length, hash);

    if (interned != NULL) {
        free(start);
        return interned;
    }

    return alloc_str(start, length, hash);
}

ObjStr* cp_str(const char* start, int length) {
    uint32_t hash = calc_str_hash(start, length);
    ObjStr* interned = dict_get_str(&vm.strings, start, length, hash);

    if (interned != NULL) {
        return interned;
    }

    char* new_str = REALLOC_ARR(char, NULL, length + 1);
    memcpy(new_str, start, length);
    new_str[length] = '\0';
    return alloc_str(new_str, length, hash);
}

void free_object(Obj* obj) {
    switch(obj->type) {
        case OBJ_STR: {
            ObjStr* str = (ObjStr*)obj;
            free(str->chars);
            free(str);
            break;                        
        }
        case OBJ_FUNC: {
            ObjFunc* fn = (ObjFunc*)obj;
            free(fn->name);
            free_ops(&fn->ops);
            free(fn);
            break;
        }
        case OBJ_NATIVE: {
            ObjNative* nat = (ObjNative*)obj;
            free(nat);
            break;
        }
        case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*)obj;
            free(closure->upvalues);
            free(closure);
            break;
        }
        case OBJ_UPVALUE: {
            ObjUpvalue* upvalue = (ObjUpvalue*)obj;
            free(upvalue);
            break;
        }
    }
}

void free_objects() {
    Obj* obj = vm.objects;
    while(obj != NULL) {
        Obj* next = obj->next;
        free_object(obj);
        obj = next;
    }
}

ObjStr* alloc_str_no_gc(char* start, int length) {
    ObjStr* str = ALLOCATE_OBJ_NO_GC(ObjStr, OBJ_STR);
    str->length = length;
    str->chars = start;
    str->hash = calc_str_hash(start, length);

    return str;
}

ObjFunc* create_func() {
    ObjFunc* fn = (ObjFunc*)ALLOCATE_OBJ(ObjFunc, OBJ_FUNC);
    fn->arity = 0;
    fn->name = NULL;
    fn->upvalue_count = 0;
    init_ops(&fn->ops);
    return fn;
}

ObjNative* create_native_func(NativeFn fn) {
    ObjNative* nat = (ObjNative*)ALLOCATE_OBJ(ObjNative, OBJ_NATIVE); 
    nat->fn = fn;
    return nat;
}

ObjClosure* create_closure(ObjFunc* fn) {
    ObjClosure* closure = (ObjClosure*)ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE); 
    closure->fn = fn;

    ObjUpvalue** upvalues = REALLOC_ARR(ObjUpvalue*, NULL, fn->upvalue_count);
    for (int i = 0; i < fn->upvalue_count; i++) {
        upvalues[i] = NULL;
    }
    closure->upvalues = upvalues;
    closure->upvalue_count = fn->upvalue_count;

    return closure;
}

ObjUpvalue* create_upvalue(Val* slot) {
    ObjUpvalue* upvalue = (ObjUpvalue*)ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE); 
    upvalue->slot = slot;
    return upvalue;
}
