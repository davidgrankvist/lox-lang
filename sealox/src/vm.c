#include <stdio.h>
#include "vm.h"
#include "dev.h"

#define CONSUME_OP() (*vm.pc++)
#define CONSUME_CONST() (vm.ops->constants.vals[CONSUME_OP()])
#define BINARY_OP(o) \
    do { \
        Val b = pop_val(); \
        Val a = pop_val(); \
        push_val(a o b); \
    } while(false)
        

VmState vm;

void reset_stack() {
    vm.top = vm.stack; 
}

void init_vm() {
    reset_stack();
}

void free_vm() {
}

void push_val(Val val) {
    *vm.top = val;
    vm.top++;
}

Val pop_val() {
   vm.top--;
   return *vm.top;
}

static IntrResult run() {
    bool keep_going = true;
    while(keep_going) {
        uint8_t op;
#ifdef DEBUG_VM
    printf("        ");
    for (Val* ptr = vm.stack; ptr < vm.top; ptr++) {
        printf("[");
        print_val(*ptr);
        printf("]");
    }
    printf("\n");
    disas_op_at(vm.ops, (int)(vm.pc - vm.ops->ops));
#endif
        switch(op = CONSUME_OP()) {
            case OP_CONST:
                push_val(CONSUME_CONST());
                break;
            case OP_RETURN: {
                print_val(pop_val());
                printf("\n");
                keep_going = false;
                break;
            }
            case OP_NEGATE:
                push_val(-pop_val());
                break;
            case OP_ADD: 
                BINARY_OP(+);
                break;
            case OP_SUBTRACT: 
                BINARY_OP(-);
                break;
            case OP_MULTIPLY: 
                BINARY_OP(*);
                break;
            case OP_DIVIDE: 
                BINARY_OP(/);
                break;
            default:
                keep_going = false;
                break;
        }
    }
    return INTR_OK;
}

IntrResult interpret(Ops* ops) {
    vm.ops = ops;
    vm.pc = vm.ops->ops; 
    return run();
}
