#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "vm.h"
#include "dev.h"
#include "compiler.h"
#include "ops.h"
#include "memory.h"

#define CONSUME_OP() (*vm.pc++)
#define CONSUME_CONST() (vm.ops->constants.vals[CONSUME_OP()])
#define BINARY_OP(mk_val, o) \
    do { \
        if (!IS_NUM(peek_val(0)) || !IS_NUM(peek_val(1))) { \
            run_err("Operands must be numbers"); \
            return INTR_RUN_ERR; \
        } \
        double b = UNWRAP_NUM(pop_val()); \
        double a = UNWRAP_NUM(pop_val()); \
        push_val(mk_val(a o b)); \
    } while(false)

VmState vm;

void reset_stack() {
    vm.top = vm.stack; 
}

void init_vm() {
    reset_stack();
    vm.objects = NULL;
}

void free_vm() {
    free_objects();
}

void push_val(Val val) {
    *vm.top = val;
    vm.top++;
}

Val pop_val() {
   vm.top--;
   return *vm.top;
}

Val peek_val(int dist) {
    return vm.top[-(dist + 1)];
}

void run_err(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t op = vm.pc - vm.ops->ops - 1;
    int line = vm.ops->lines[op];
    fprintf(stderr, "[line %d] in script \n", line);
    reset_stack();
}

bool is_falsey(Val val) {
    return IS_NIL(val) || (IS_BOOL(val) && !UNWRAP_BOOL(val));
}

bool are_equal(Val a, Val b) {
    if (a.type != b.type) {
        return false;
    }

    switch (a.type) {
        case VAL_NIL:
            return true;
        case VAL_BOOL:
            return UNWRAP_BOOL(a) == UNWRAP_BOOL(b);
        case VAL_NUM:
            return UNWRAP_NUM(a) == UNWRAP_NUM(b);
        case VAL_OBJ: {
            ObjStr* a_str = UNWRAP_STR(a);
            ObjStr* b_str = UNWRAP_STR(b);
            return a_str->length == b_str->length
                && memcmp(a_str->chars, b_str->chars, a_str->length) == 0;
        }
        default:
            return false;
    }
}

void concat() {
    Val b = pop_val();  
    Val a = pop_val();  
    ObjStr* a_str = UNWRAP_STR(a);
    ObjStr* b_str = UNWRAP_STR(b);

    int length = a_str->length + b_str->length;
    char* new_str = REALLOC_ARR(char, NULL, length + 1);
    memcpy(new_str, a_str->chars, a_str->length);
    memcpy(new_str + a_str->length, b_str->chars, b_str->length);
    new_str[length] = '\0';

    ObjStr* result = alloc_str(new_str, length);
    push_val(MK_OBJ_VAL((Obj*)result));
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
            case OP_TRUE:
                push_val(MK_BOOL_VAL(true));
                break;
            case OP_FALSE:
                push_val(MK_BOOL_VAL(false));
                break;
            case OP_NIL:
                push_val(MK_NIL_VAL);
                break;
            case OP_RETURN: {
                print_val(pop_val());
                printf("\n");
                keep_going = false;
                break;
            }
            case OP_NEGATE:
                if (!IS_NUM(peek_val(0))) {
                   run_err("Operand must be a number"); 
                   return INTR_RUN_ERR;
                }
                push_val(MK_NUM_VAL(-UNWRAP_NUM(pop_val())));
                break;
            case OP_NOT:
                push_val(MK_BOOL_VAL(is_falsey(pop_val())));
                break;
            case OP_ADD: 
                if (IS_STR(peek_val(0)) && IS_STR(peek_val(1))) {
                    concat();
                } else {
                    BINARY_OP(MK_NUM_VAL, +);
                }
                break;
            case OP_SUBTRACT: 
                BINARY_OP(MK_NUM_VAL, -);
                break;
            case OP_MULTIPLY: 
                BINARY_OP(MK_NUM_VAL, *);
                break;
            case OP_DIVIDE: 
                BINARY_OP(MK_NUM_VAL, /);
                break;
            case OP_EQUAL: {
                Val a = pop_val();
                Val b = pop_val();
                push_val(MK_BOOL_VAL(are_equal(a, b)));
                break; 
            }
            case OP_LESS:
                BINARY_OP(MK_BOOL_VAL, <);
                break;
            case OP_GREATER:
                BINARY_OP(MK_BOOL_VAL, >);
                break;
            default:
                keep_going = false;
                break;
        }
    }
    return INTR_OK;
}

IntrResult run_ops(Ops* ops) {
    vm.ops = ops;
    vm.pc = vm.ops->ops; 
    return run();
}

IntrResult interpret(char* program) {
    Ops ops;
    init_ops(&ops);

    if (!compile(program, &ops)) {
        free_ops(&ops);
        return INTR_COMP_ERR;
    }

    vm.ops = &ops;
    vm.pc = vm.ops->ops;

    IntrResult result = run();

    free_ops(&ops);
    return result;
}
