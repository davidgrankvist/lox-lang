#include <stdio.h>
#include "dev.h"
#include "ops.h"

void disas_ops(Ops* ops, const char* name) {
   printf("-- %s --\n", name); 

   for (int pos = 0; pos < ops->count;) {
        pos = disas_op_at(ops, pos);
   }
}

int disas_simple(const char* name, int pos) {
    printf("%s\n", name);
    return pos + 1;
}

int disas_const(const char* name, int pos, Ops* ops) {
    uint8_t i_constant = ops->ops[pos + 1];
    Val val = ops->constants.vals[i_constant];
    printf("%s %4d ", name, i_constant);
    print_val(val);
    printf("\n");
    return pos + 2;
}

void print_obj(Val val) {
    switch(OBJ_TYPE(val)) {
        case OBJ_STR: {
            char* chars = UNWRAP_STR_CHARS(val);
            printf("%s", chars); 
            break;
        }
        default:
            printf("<unknown obj>"); 
            break;
    }
}

void print_val(Val val) {
    if (IS_NUM(val)) {
        printf("%g", UNWRAP_NUM(val));
    } else if(IS_BOOL(val)) {
        printf("%s", UNWRAP_BOOL(val) ? "true" : "false");
    } else if(IS_NIL(val)) {
        printf("nil");
    } else if(IS_OBJ(val)) {
        print_obj(val);    
    } else {
        printf("<unknown val>");
    }
}

int disas_op_at(Ops* ops, int pos) {
    int line = ops->lines[pos];
    printf("%04d %4d ", pos, line);

    uint8_t op = ops->ops[pos];
    int next_pos = pos;

    switch (op) {
        case OP_RETURN:
            next_pos = disas_simple("OP_RETURN", pos);
            break;
        case OP_CONST:
            next_pos = disas_const("OP_CONST", pos, ops);
            break;
        case OP_TRUE:
            next_pos = disas_simple("OP_TRUE", pos);
            break;
        case OP_FALSE:
            next_pos = disas_simple("OP_FALSE", pos);
            break;
        case OP_NIL:
            next_pos = disas_simple("OP_NIL", pos);
            break;
        case OP_NEGATE:
            next_pos = disas_simple("OP_NEGATE", pos);
            break;
        case OP_ADD:
            next_pos = disas_simple("OP_ADD", pos);
            break;
        case OP_SUBTRACT:
            next_pos = disas_simple("OP_SUBTRACT", pos);
            break;
        case OP_MULTIPLY:
            next_pos = disas_simple("OP_MULTIPLY", pos);
            break;
        case OP_DIVIDE:
            next_pos = disas_simple("OP_DIVIDE", pos);
            break;
        case OP_NOT:
            next_pos = disas_simple("OP_NOT", pos);
            break;
        case OP_EQUAL:
            next_pos = disas_simple("OP_EQUAL", pos);
            break;
        case OP_LESS:
            next_pos = disas_simple("OP_LESS", pos);
            break;
        case OP_GREATER:
            next_pos = disas_simple("OP_GREATER", pos);
            break;
        case OP_PRINT:
            next_pos = disas_simple("OP_PRINT", pos);
            break;
        case OP_POP:
            next_pos = disas_simple("OP_POP", pos);
            break;
        case OP_DEFINE_GLOBAL:
            next_pos = disas_simple("OP_DEFINE_GLOBAL", pos);
            break;
        case OP_GET_GLOBAL:
            next_pos = disas_simple("OP_GET_GLOBAL", pos);
            break;
        case OP_SET_GLOBAL:
            next_pos = disas_simple("OP_SET_GLOBAL", pos);
            break;
        case OP_GET_LOCAL:
            next_pos = disas_simple("OP_GET_LOCAL", pos);
            break;
        case OP_SET_LOCAL:
            next_pos = disas_simple("OP_SET_LOCAL", pos);
            break;
        default:
            printf("Unknown op code %d\n", op);
            next_pos++;
    }
    return next_pos;
}
