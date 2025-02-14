#ifndef vm_h
#define vm_h

#include "common.h"
#include "ops.h"
#include "dev.h"
#include "dict.h"

#define STACK_SIZE 256

typedef struct {
    Ops* ops;
    uint8_t* pc;
    Val stack[STACK_SIZE];
    Val* top;
    Dict strings;
    Obj* objects;
    Dict globals;
} VmState;

extern VmState vm;

typedef enum {
    INTR_OK,
    INTR_COMP_ERR,
    INTR_RUN_ERR
} IntrResult;

void init_vm();
void free_vm();
IntrResult interpret(char* program);
IntrResult run_ops(Ops* ops);

void push_val(Val val);
Val pop();

#endif
