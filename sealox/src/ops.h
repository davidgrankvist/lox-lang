#ifndef ops_h
#define ops_h

#include "common.h"

typedef enum {
    OP_RETURN,
    OP_CONST,
    OP_NEGATE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE
} OpCode;

typedef double Val;

typedef struct {
    int count;
    int capacity;
    Val* vals;
} Vals;

typedef struct {
    int count;
    int capacity;
    uint8_t* ops;
    Vals constants;
    int* lines;
} Ops;

void init_ops(Ops* ops);
void free_ops(Ops* ops);
void append_op(Ops* ops, uint8_t byte, int line);

void init_vals(Vals* vals);
void free_vals(Vals* vals);
void append_val(Vals* vals, Val val);

int append_const(Ops* ops, Val val);

#endif
