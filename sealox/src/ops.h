#ifndef ops_h
#define ops_h

#include "common.h"

typedef enum {
    OP_RETURN,
    OP_CONST,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_NEGATE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_PRINT,
    OP_POP,
    OP_DEFINE_GLOBAL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_JMP_IF_FALSE,
    OP_JMP,
    OP_LOOP,
    OP_CALL,
    OP_CLOSURE,
    OP_GET_UPVALUE,
    OP_SET_UPVALUE,
} OpCode;

typedef enum {
    OBJ_STR,
    OBJ_FUNC,
    OBJ_NATIVE,
    OBJ_CLOSURE,
    OBJ_UPVALUE,
} ObjType;

typedef struct Obj {
    ObjType type; 
    struct Obj* next;
} Obj;

typedef struct {
    Obj obj;
    int length;
    char* chars;
    uint32_t hash;
} ObjStr;

typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUM,
    VAL_OBJ
} ValType;

typedef struct {
    ValType type;
    union {
        bool boolean;
        double number;
        Obj* obj;
    } unwrap;
} Val;

#define MK_BOOL_VAL(b) ((Val){VAL_BOOL, { .boolean = b }})
#define MK_NUM_VAL(n) ((Val){VAL_NUM, { .number = n }})
#define MK_NIL_VAL ((Val){VAL_NIL, { .number = 0 }})
#define MK_OBJ_VAL(o) ((Val){VAL_OBJ, { .obj = o }})

#define IS_BOOL(v) ((v).type == VAL_BOOL)
#define IS_NUM(v) ((v).type == VAL_NUM)
#define IS_NIL(v) ((v).type == VAL_NIL)
#define IS_OBJ(v) ((v).type == VAL_OBJ)

#define UNWRAP_BOOL(v) ((v).unwrap.boolean)
#define UNWRAP_NUM(v) ((v).unwrap.number)
#define UNWRAP_OBJ(v) ((v).unwrap.obj)

#define OBJ_TYPE(v) (UNWRAP_OBJ(v)->type)

static inline bool is_obj_type(Val v, ObjType t) {
    return IS_OBJ(v) && UNWRAP_OBJ(v)->type == t;
}
#define IS_STR(v) is_obj_type(v, OBJ_STR)
#define UNWRAP_STR(v) ((ObjStr*)(UNWRAP_OBJ(v)))
#define UNWRAP_STR_CHARS(v) (UNWRAP_STR(v)->chars)

#define IS_FUNC(v) is_obj_type(v, OBJ_FUNC)
#define UNWRAP_FUNC(v) ((ObjFunc*)(UNWRAP_OBJ(v)))

#define IS_NATIVE(v) is_obj_type(v, OBJ_NATIVE)
#define UNWRAP_NATIVE_FN(v) ((ObjNative*)(UNWRAP_OBJ(v)))

#define IS_CLOSURE(v) is_obj_type(v, OBJ_CLOSURE)
#define UNWRAP_CLOSURE(v) ((ObjClosure*)(UNWRAP_OBJ(v)))

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

typedef struct {
    Obj obj;
    int arity;
    Ops ops;
    ObjStr* name;
    int upvalue_count;
} ObjFunc;

typedef struct ObjUpvalue {
    Obj obj;
    Val* slot;
} ObjUpvalue;

typedef struct {
    Obj obj;
    ObjFunc* fn;
    ObjUpvalue** upvalues;
    int upvalue_count;
} ObjClosure;

typedef Val (*NativeFn)(int argc, Val* args);

typedef struct {
    Obj obj;
    NativeFn fn;
} ObjNative;

void init_ops(Ops* ops);
void free_ops(Ops* ops);
void append_op(Ops* ops, uint8_t byte, int line);

void init_vals(Vals* vals);
void free_vals(Vals* vals);
void append_val(Vals* vals, Val val);

int append_const(Ops* ops, Val val);

#endif
