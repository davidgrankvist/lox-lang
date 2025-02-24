#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "vm.h"
#include "dev.h"
#include "compiler.h"
#include "ops.h"
#include "memory.h"

#define CONSUME_OP() (*frame->pc++)
#define CONSUME_OP16() \
    (vm.pc += 2, (uint16_t)((frame->pc[-2] << 8) | (frame->pc[-1])))
#define CONSUME_CONST() (frame->fn->ops.constants.vals[CONSUME_OP()])
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

static void define_native(const char* name, NativeFn fn);
static Val clock_native(int argc, Val* args);

VmState vm;

void reset_stack() {
    vm.top = vm.stack; 
    vm.frame_count = 0;
}

void init_vm() {
    reset_stack();
    dict_init(&vm.strings);
    dict_init(&vm.globals);
    vm.objects = NULL;

    define_native("clock", clock_native);
}

void free_vm() {
    dict_free(&vm.strings);
    dict_free(&vm.globals);
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

    // stack trace
    for (int i = vm.frame_count - 1; i >= 0; i--) {
        CallFrame* frame = &vm.frames[i];
        ObjFunc* fn = frame->fn;
        size_t instruction = frame->pc - fn->ops.ops - 1; // -1 because pc is already at the next one
        fprintf(stderr, "[line %d] in ", fn->ops.lines[instruction]);
        if (fn->name == NULL) {
            fprintf(stderr, "script\n");
        } else {
            fprintf(stderr, "%s()\n", fn->name->chars);
        }
    }

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

    ObjStr* result = take_str(new_str, length);
    push_val(MK_OBJ_VAL((Obj*)result));
}

static bool call(ObjFunc* fn, int argc) {
    if (argc != fn->arity) {
        run_err("Unexpected number of function call arguments. Expected %d, but received %d", fn->arity, argc);
        return false;
    }
    if (vm.frame_count == MAX_FRAMES) {
        run_err("Stack overflow. At most %d call frames are allowed. Sorry.", MAX_FRAMES);
        return false;
    }
    CallFrame* frame = &vm.frames[vm.frame_count++];
    frame->fn = fn;
    frame->pc = fn->ops.ops;
    frame->slots = vm.top - argc - 1;
    return true;
}

static Val clock_native(int argc, Val* args) {
    return MK_NUM_VAL((double)clock() / CLOCKS_PER_SEC);
}

static void define_native(const char* name, NativeFn fn) {
    // push / pop to make sure GC picks up the allocated string / function
    push_val(MK_OBJ_VAL((Obj*)cp_str(name, (int)strlen(name))));
    push_val(MK_OBJ_VAL((Obj*)create_native_func(fn)));

    // declare the native function as a global
    dict_put(&vm.globals, UNWRAP_STR(vm.stack[0]), vm.stack[1]);

    pop_val();
    pop_val();
}

static bool call_val(Val callee, int argc) {
    if (IS_OBJ(callee)) {
        switch(OBJ_TYPE(callee)) {
            case OBJ_FUNC:
                return call(UNWRAP_FUNC(callee), argc);
            case OBJ_NATIVE: {
                NativeFn fn = UNWRAP_NATIVE_FN(callee)->fn;
                Val result = fn(argc, vm.top - argc);
                push_val(result);
                return true;
            }
            default:
                break;
        }
    }
    run_err("Can only call functions and classes");
    return false;
}

static IntrResult run() {
    CallFrame* frame = &vm.frames[vm.frame_count - 1];
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
    disas_op_at(&frame->fn->ops, (int)(frame->pc - frame->fn->ops.ops));
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
                Val result = pop_val(); 
                vm.frame_count--;
                if (vm.frame_count == 0) {
                    pop_val(); // pop main function
                    return INTR_OK;
                }

                vm.top = frame->slots;
                push_val(result);
                frame = &vm.frames[vm.frame_count - 1];
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
            case OP_PRINT:
                print_val(pop_val());
                printf("\n");
                break;
            case OP_POP:
                pop_val();
                break;
            case OP_DEFINE_GLOBAL: {
                ObjStr* name = UNWRAP_STR(CONSUME_CONST());
                dict_put(&vm.globals, name, pop_val());
                break;
            }
            case OP_GET_GLOBAL: {
                ObjStr* name = UNWRAP_STR(CONSUME_CONST());
                Val val;
                if (!dict_get(&vm.globals, name, &val)) {
                    run_err("Unable to read undefined variable '%s'", name->chars);
                    return INTR_RUN_ERR;
                }
                push_val(val);
                break;
            }
            case OP_SET_GLOBAL: {
                ObjStr* name = UNWRAP_STR(CONSUME_CONST());
                if (!dict_has(&vm.globals, name)) {
                    dict_del(&vm.globals, name);
                    run_err("Unable to assign to undefined variable '%s'", name->chars);
                    return INTR_RUN_ERR;
                }
                dict_put(&vm.globals, name, peek_val(0));
                break;
            }
            case OP_GET_LOCAL: {
                uint8_t slot = CONSUME_OP();
                push_val(frame->slots[slot]);
                break;
            }
            case OP_SET_LOCAL: {
                uint8_t slot = CONSUME_OP();
                frame->slots[slot] = peek_val(0);
                break;
            }
            case OP_JMP_IF_FALSE: {
                uint16_t offset = CONSUME_OP16();
                if (is_falsey(peek_val(0))) {
                    frame->pc += offset;
                }
                break;
            }
            case OP_JMP: {
                uint16_t offset = CONSUME_OP16();
                frame->pc += offset;
                break;
            }
            case OP_LOOP: {
                uint16_t offset = CONSUME_OP16();
                frame->pc -= offset;
                break;
            }
            case OP_CALL: {
                int argc = CONSUME_OP();
                if (!call_val(peek_val(argc), argc)) {
                    return INTR_RUN_ERR;
                }
                frame = &vm.frames[vm.frame_count - 1];
                break;
            }
            default:
                keep_going = false;
                break;
        }
    }
    return INTR_OK;
}

IntrResult interpret(char* program) {
    ObjFunc* fn = compile(program);
    if (fn == NULL) {
        return INTR_COMP_ERR;
    }

    push_val(MK_OBJ_VAL((Obj*)fn));
    call(fn, 0); // call implicit "main"

    return run();
}
