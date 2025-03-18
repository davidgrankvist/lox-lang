#ifndef vm_h
#define vm_h

#include "common.h"
#include "ops.h"
#include "dev.h"
#include "dict.h"

#define MAX_FRAMES 64
#define STACK_SIZE (MAX_FRAMES * UINT8_COUNT)

typedef struct {
    ObjClosure* closure;
    uint8_t* pc;
    Val* slots;
} CallFrame;

typedef struct {
    Ops* ops;
    uint8_t* pc;

    Val stack[STACK_SIZE];
    Val* top;

    Dict strings;
    Obj* objects;
    Dict globals;

    CallFrame frames[MAX_FRAMES];
    int frame_count;
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
