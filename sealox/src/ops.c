#include "ops.h"
#include "memory.h"

void init_ops(Ops* ops) {
    ops->count = 0;
    ops->capacity = 0;
    ops->ops = NULL;
    ops->lines = NULL;
    init_vals(&ops->constants);
}

void free_ops(Ops* ops) {
    free(ops->ops);
    free_vals(&ops->constants);
    free(ops->lines);
    init_ops(ops);
}

void append_op(Ops* ops, uint8_t byte, int line) {
   if (ops->count + 1 > ops->capacity) {
       ops->capacity = CALC_CAP(ops->capacity);
       ops->ops = REALLOC_ARR(uint8_t, ops->ops, ops->capacity);
       ops->lines = REALLOC_ARR(int, ops->lines, ops->capacity);
   }
   ops->ops[ops->count] = byte;
   ops->lines[ops->count] = line;
   ops->count++;
}


void init_vals(Vals* vals) {
    vals->count = 0;
    vals->capacity = 0;
    vals->vals = NULL;
}

void free_vals(Vals* vals) {
    free(vals->vals);
    init_vals(vals);
}

void append_val(Vals* vals, Val val) {
   if (vals->count + 1 > vals->capacity) {
       vals->capacity = CALC_CAP(vals->capacity);
       vals->vals = REALLOC_ARR(Val, vals->vals, vals->capacity);
   }
   vals->vals[vals->count] = val;
   vals->count++;
}

int append_const(Ops* ops, Val val) {
    append_val(&ops->constants, val);
    return ops->constants.count - 1;
}

Obj* allocate_obj (size_t size, ObjType type) {
    Obj* obj = (Obj*)realloc_arr(NULL, size);
    obj->type = type;

    return obj;
}

#define ALLOCATE_OBJ(type, otype) \
    (type*)allocate_obj(sizeof(type), otype)

ObjStr* alloc_str(char* start, int length) {
    ObjStr* str = ALLOCATE_OBJ(ObjStr, OBJ_STR);
    str->length = length;
    str->chars = start;

    return str;
}

