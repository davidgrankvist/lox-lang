#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "compiler.h"
#include "scanner.h"
#include "memory.h"
#ifdef DEBUG_COMP
#include "dev.h"
#endif

typedef struct {
    Token name;
    int depth;    
} Local;

typedef struct {
    Local locals[UINT8_COUNT]; 
    int local_count;
    int scope_depth;
} Compiler;

typedef struct {
    Token curr;
    Token prev;
    bool err;
    bool panic;
} Parser;

Parser parser;
Ops* ops_ptr;
Compiler* comp = NULL;

typedef enum {
    P_NONE,
    P_ASSIGN,
    P_OR,
    P_AND,
    P_EQ,
    P_COMP,
    P_TERM,
    P_FACTOR,
    P_UNARY,
    P_CALL,
    P_PRIMARY
} Prec;

typedef void (*Pfn)();

typedef struct {
    Pfn prefix;
    Pfn infix;
    Prec prec;
} Rule;

static void parse_expr();
static void parse_expr_stmt();
static void parse_var_decl();
static Rule* rule_of(TokenType type);
static void parse_prec(Prec prec);
static void parse_decl();
static void parse_stmt();
static bool id_equal(Token* first, Token* second);
static void mark_initialized();

void init_comp(Compiler* compiler) {
    compiler->local_count = 0;
    compiler->scope_depth = 0;
    comp = compiler;
}

void err_at(Token* token, const char* msg) {
    if (parser.panic) {
        return;
    }
    parser.panic = true;
    fprintf(stderr, "[line %d] Error", token->line);
    switch(token->type) {
        case TOKEN_EOF:
            fprintf(stderr, " at end");
            break;
        case TOKEN_ERROR:
            break;
        default:
            fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", msg);
    parser.err = true;
}

void err(const char* msg) {
    err_at(&parser.prev, msg); 
}

void err_curr(const char* msg) {
    err_at(&parser.curr, msg); 
}

void advance() {
    parser.prev = parser.curr;

    while(true) {
        parser.curr = scan_token();
        if (parser.curr.type != TOKEN_ERROR) {
            break;
        }

        err_curr(parser.curr.start);
    }
}

void consume(TokenType type, char* msg) {
    if (parser.curr.type == type) {
        advance();
        return;
    }

    err_curr(msg);
}

static bool match(TokenType type) {
    if (parser.curr.type == type) {
        advance();
        return true;
    }

    return false;
}

static bool check(TokenType type) {
    return parser.curr.type == type;
}

Ops* curr_ops() {
    return ops_ptr;
}

/*
 * If a statement is broken, we avoid cascading errors
 * by aborting it and searching for a new statement.
 */
static void synch() {
    parser.panic = true;

    while (!check(TOKEN_EOF)) {
        // end of statement boundary
        if (parser.prev.type == TOKEN_SEMICOLON) {
            return;
        }
        // beginning of statement boundary
        switch (parser.curr.type) {
            case TOKEN_CLASS:
            case TOKEN_FUN:
            case TOKEN_VAR:
            case TOKEN_FOR:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_PRINT:
            case TOKEN_RETURN:
                return;
        }

        advance();
    }
}


void emit(uint8_t byte) {
    append_op(curr_ops(), byte, parser.prev.line);
}

void emit2(uint8_t b1, uint8_t b2) {
    emit(b1);
    emit(b2);
}

void emit_ret() {
    emit(OP_RETURN);
}

void end_comp() {
    emit_ret();
#ifdef DEBUG_COMP
    if (parser.err) {
        disas_ops(curr_ops(), "code");
    }
#endif
}

uint8_t mk_const(Val val) {
    int i_const = append_const(curr_ops(), val);
    if (i_const > UINT8_MAX) {
        err("Too many constants");
        return 0;
    }

    return (uint8_t)i_const;
}

void emit_const(Val val) {
    emit2(OP_CONST, mk_const(val));
}

void parse_num() {
    double val = strtod(parser.prev.start, NULL); 
    emit_const(MK_NUM_VAL(val));
}

void parse_bool() {
    switch(parser.prev.type) {
        case TOKEN_TRUE:
            emit(OP_TRUE);
            break;
        case TOKEN_FALSE:
            emit(OP_FALSE);
            break;
    }
}

void parse_nil() {
    emit(OP_NIL);
}

static void parse_prec(Prec prec) {
    advance();
    Pfn fn = rule_of(parser.prev.type)->prefix;
    if (fn == NULL) {
       err("Unknown expression"); 
       return;
    }

    bool can_assign = prec <= P_ASSIGN;
    fn(can_assign);

    while (prec <= rule_of(parser.curr.type)->prec) {
        advance();
        Pfn ifn = rule_of(parser.prev.type)->infix;
        ifn();
    }

    if (can_assign && match(TOKEN_EQUAL)) {
        err("Invalid assignment target");
    }

}

static void parse_expr() {
    parse_prec(P_ASSIGN);  
}

void parse_group() {
    parse_expr();
    consume(TOKEN_PAREN_END, "Expected ')'");
}

void parse_unary() {
    TokenType type = parser.prev.type;

    parse_prec(P_UNARY);

    switch(type) {
        case TOKEN_MINUS:
            emit(OP_NEGATE);
            break;
        case TOKEN_BANG:
            emit(OP_NOT);
            break;
    }
}

void parse_binary() {
    TokenType op = parser.prev.type;
    Rule* rule = rule_of(op);
    parse_prec((Prec)(rule->prec + 1)); // left-associative

    switch (op) {
        case TOKEN_PLUS:
            emit(OP_ADD);
            break;
        case TOKEN_MINUS:
            emit(OP_SUBTRACT);
            break;
        case TOKEN_STAR:
            emit(OP_MULTIPLY);
            break;
        case TOKEN_SLASH:
            emit(OP_DIVIDE);
            break;
        case TOKEN_EQUAL_EQUAL:
            emit(OP_EQUAL);
            break;
        case TOKEN_BANG_EQUAL:
            // a != b -> !(a == b)
            emit2(OP_EQUAL, OP_NOT);
            break;
        case TOKEN_LESS:
            emit(OP_LESS);
            break;
        case TOKEN_LESS_EQUAL:
            // a <= b -> !(a > b)
            emit2(OP_GREATER, OP_NOT);
            break;
        case TOKEN_GREATER:
            emit(OP_GREATER);
            break;
        case TOKEN_GREATER_EQUAL:
            // a >= b -> !(a < b)
            emit2(OP_LESS, OP_NOT);
            break;
    }
}

void parse_str() {
    emit_const(MK_OBJ_VAL((Obj*)cp_str(parser.prev.start + 1, parser.prev.length - 2)));
}

void parse_print() {
    parse_expr(); 
    consume(TOKEN_SEMICOLON, "Expected ';' at the end of print statement");
    emit(OP_PRINT);
}

static void begin_scope() {
    comp->scope_depth++;    
}

static void end_scope() {
   comp->scope_depth--; 

   // clear local variables from the ended scope
   while (comp->local_count > 0 &&
           comp->locals[comp->local_count - 1].depth >
            comp->scope_depth) {
        emit(OP_POP);
        comp->local_count--;
   }
}

static void parse_block() {
    while(!check(TOKEN_CURLY_END) && !check(TOKEN_EOF)) {
        parse_decl();
    }

    consume(TOKEN_CURLY_END, "Expected '}' at the end of block");
}

/*
 * Emit a preliminary instruction, since we need to parse the statement
 * before we know where to jump.
 */
static int emit_jmp(uint8_t op) {
    emit(op);
    // placeholders
    emit2(0xFF, 0xFF);
    // index of first placeholder byte
    return curr_ops()->count - 2;
}

static void patch_jmp(int i_jmp_val) {
    /*
     * Subtract i_jmp_val to get distance from the start of the jump value
     * to the end of block. Subtract another 2 to only get the block length.
     */
    int jmp_dist = curr_ops()->count - i_jmp_val - 2; 

    if (jmp_dist > UINT16_MAX) {
        err("Uh oh, you can't jump that far in a condition. 16-bit numbers are used for the jump destination. Sorry.");
    }

    // the upper 8 bits are emitted first
    curr_ops()->ops[i_jmp_val] = (jmp_dist >> 8) & 0xFF;
    curr_ops()->ops[i_jmp_val + 1] = jmp_dist & 0xFF;
}

static void parse_if() {
    consume(TOKEN_PAREN_START, "Expected '(' before if condition");
    parse_expr();
    consume(TOKEN_PAREN_END, "Expected ')' after if condition");

    int if_jmp = emit_jmp(OP_JMP_IF_FALSE);
    emit(OP_POP); // pop condition at beginning of if
    parse_stmt();

    // add a jump at the end of the if block to skip the else block
    int else_jmp = emit_jmp(OP_JMP);

    patch_jmp(if_jmp);
    emit(OP_POP); // pop condition at beginning of else, since we jumped past the other pop

    if (match(TOKEN_ELSE)) {
        parse_stmt();
    }
    patch_jmp(else_jmp);
}

static void emit_loop(int loop_start) {
      emit(OP_LOOP);

      int offset = curr_ops()->count - loop_start + 2;
      if (offset > UINT16_MAX) {
        err("Uh oh, you can't have such a large while loop body. 16-bit numbers are used for the jump destination. Sorry.");
      }

      emit2((offset >> 8) & 0xFF, offset & 0xFF);
}

static void parse_while() {
    int loop_start = curr_ops()->count;
    consume(TOKEN_PAREN_START, "Expected '(' before while condition"); 
    parse_expr();
    consume(TOKEN_PAREN_END, "Expected ')' after while condition"); 

    int exit_jmp = emit_jmp(OP_JMP_IF_FALSE);
    emit(OP_POP);
    parse_stmt();
    emit_loop(loop_start);
    patch_jmp(exit_jmp);
    emit(OP_POP);
}

static void parse_for() {
    // make sure initializer variable is in a dedicated scope
    begin_scope();
    consume(TOKEN_PAREN_START, "Expected '(' before for constructs");

    // optional initializer
    if (match(TOKEN_SEMICOLON)) {
        // no initializer
    } else if (match(TOKEN_VAR)) {
       parse_var_decl(); 
    } else {
       parse_expr_stmt(); 
    }

    int loop_start = curr_ops()->count;
    // optional loop condition
    int exit_jmp = -1;
    if (!match(TOKEN_SEMICOLON)) {
        parse_expr(); 
        consume(TOKEN_SEMICOLON, "Expected ';' after for condition");
        exit_jmp = emit_jmp(OP_JMP_IF_FALSE);
        emit(OP_POP); // pop condition before entering for block
    }

    // optional increment
    if (!match(TOKEN_PAREN_END)) {
        /*
         * Use jumps in order to execute the increment
         * after the for loop body.
         */
        int body_jmp = emit_jmp(OP_JMP); // jump to skip increment
        int inc_start = curr_ops()->count;
        // Consume increment. No semicolon, but otherwise like an expression statement
        parse_expr();
        emit(OP_POP);
        consume(TOKEN_PAREN_END, "Expected ')' after for constructs");

        emit_loop(loop_start); // jump back to the condition
        loop_start = inc_start; // after loop body, jump to increment
        patch_jmp(body_jmp);
    }

    parse_stmt();
    emit_loop(loop_start); 
    // check for optional condition exit
    if (exit_jmp != -1) {
        patch_jmp(exit_jmp);
        emit(OP_POP); // pop condition when leaving for block
    }
    end_scope();
}

static void parse_expr_stmt() {
    parse_expr();
    consume(TOKEN_SEMICOLON, "Expected ';' at the end of statement");
    emit(OP_POP);
}

void parse_stmt() {
    if (match(TOKEN_PRINT)) {
        parse_print();
    } else if(match(TOKEN_IF)) {
        parse_if();
    } else if(match(TOKEN_WHILE)) {
        parse_while();
    } else if(match(TOKEN_FOR)) {
        parse_for();
    } else if (match(TOKEN_CURLY_START)) {
        begin_scope();
        parse_block();
        end_scope();
    } else {
        parse_expr_stmt();
    }
}

void define_var(uint8_t i_val) {
    /*
     * No define is necessary for local variables,
     * as they are stored directly on the VM stack.
     */
    if (comp->scope_depth > 0) {
        mark_initialized();
        return;
    }

    emit2(OP_DEFINE_GLOBAL, i_val); 
}

uint8_t identifier_constant(Token* token) {
    return mk_const(MK_OBJ_VAL((Obj*)cp_str(token->start, token->length)));
}

static int resolve_local(Compiler* compiler, Token* token) {
    for (int i = compiler->local_count - 1; i >= 0; i--) {
        Local* local = &compiler->locals[i];
        if (id_equal(&local->name, token)) {
            if (local->depth == -1) {
                err("Can't read local variable in its own initializer");
            }
            return i;
        }
    }
    return -1;
}

void parse_named_var(Token* token, bool can_assign) {
    uint8_t get_op;
    uint8_t set_op;

    int i_val = resolve_local(comp, token);
    if (i_val != -1) {
        get_op = OP_GET_LOCAL;
        set_op = OP_SET_LOCAL;
    } else {
        i_val = identifier_constant(token);
        get_op = OP_GET_GLOBAL;
        set_op = OP_SET_GLOBAL;
    }

    if (can_assign && match(TOKEN_EQUAL)) {
        parse_expr();
        emit2(set_op, (uint8_t)i_val);
    } else {
        emit2(get_op, (uint8_t)i_val);
    }
}

void parse_var_val(bool can_assign) {
    parse_named_var(&parser.prev, can_assign);
}

static void mark_initialized() {
    comp->locals[comp->local_count - 1].depth = comp->scope_depth;
}

static void add_local(Token token) {
    if (comp->local_count == UINT8_COUNT) {
        err("Too many local variables");
        return;
    }

    Local* local = &comp->locals[comp->local_count++];
    local->name = token;
    /* Mark as declared, but not initialized.
     * This is to avoid var a = a;
     */
    local->depth = -1;
}

static bool id_equal(Token* first, Token* second) {
    return first->length == second->length &&
        memcmp(first->start, second->start, first->length) == 0;
}

static void declare_var() {
    // global variables are late bound
    if (comp->scope_depth == 0) {
        return;
    }

    Token* name = &parser.prev;

    // look for conflicting variable names
    for (int i = comp->local_count - 1; i >= 0; i--) {
        Local* local = &comp->locals[i];
        if (local->depth != -1 && local->depth < comp->scope_depth) {
            break; 
        }

        if (id_equal(name, &local->name)) {
            err("Variable already exists in this scope");
        }
    }

    add_local(*name);
}

uint8_t parse_var(char* str) {
    consume(TOKEN_IDENTIFIER, "Expected variable identifier");

    declare_var();
    /*
     * Local variables are not looked up by name at runtime,
     * so we don't emit a named constant.
     */
    if (comp->scope_depth > 0) {
        return 0;
    }
    
    return identifier_constant(&parser.prev);
}

static void parse_var_decl() {
    uint8_t global = parse_var("Expected a variable name");

    if (match(TOKEN_EQUAL)) {
        parse_expr(); 
    } else {
        emit(OP_NIL); 
    }

    consume(TOKEN_SEMICOLON, "Expected ';' at the end of variable declaration");
    define_var(global);
}

void parse_decl() {
    if (match(TOKEN_VAR)) {
        parse_var_decl();
    } else {
        parse_stmt();
    }

    if (parser.panic) {
        synch();
    }
}

static void parse_and() {
    // skip right operand if left operand is false
    int jmp = emit_jmp(OP_JMP_IF_FALSE); 
    emit(OP_POP);
    parse_prec(P_AND);
    patch_jmp(jmp);
}

static void parse_or() {
    /*
     * Skip right operand if left operand is true.
     *
     * Instead of having a dedicated JMP_IF_TRUE, two jumps are combined to get that behavior.
     */
    int false_jmp = emit_jmp(OP_JMP_IF_FALSE);
    int true_jmp = emit_jmp(OP_JMP);

    patch_jmp(false_jmp);
    /*
     * If the left operand was false, then we pop it and let the
     * whole expression evaluate to the right operand.
     */
    emit(OP_POP);
    
    parse_prec(P_OR);

    patch_jmp(true_jmp);
}

bool compile(const char* program, Ops* ops) {
    init_scanner(program); 
    ops_ptr = ops;

    Compiler compiler;
    init_comp(&compiler);

    parser.err = false;
    parser.panic = false;

    advance();

    while (!match(TOKEN_EOF)) {
        parse_decl();
    }

    consume(TOKEN_EOF, "Expected EOF");
    end_comp();
    return !parser.err;
}

// mapping from tokens to rules
Rule rules[] = {
    [TOKEN_NUMBER]          = {parse_num, NULL, P_NONE},
    [TOKEN_STRING]          = {parse_str, NULL, P_NONE},
    [TOKEN_TRUE]            = {parse_bool, NULL, P_NONE},
    [TOKEN_FALSE]           = {parse_bool, NULL, P_NONE},
    [TOKEN_PLUS]            = {NULL, parse_binary, P_TERM},
    [TOKEN_MINUS]           = {parse_unary, parse_binary, P_TERM},
    [TOKEN_STAR]            = {NULL, parse_binary, P_FACTOR},
    [TOKEN_SLASH]           = {NULL, parse_binary, P_FACTOR},
    [TOKEN_BANG]            = {parse_unary, NULL, P_NONE},
    [TOKEN_EQUAL]           = {NULL, NULL, P_NONE},
    [TOKEN_EQUAL_EQUAL]     = {NULL, parse_binary, P_EQ},
    [TOKEN_BANG_EQUAL]      = {NULL, parse_binary, P_EQ},
    [TOKEN_LESS]            = {NULL, parse_binary, P_COMP},
    [TOKEN_LESS_EQUAL]      = {NULL, parse_binary, P_COMP},
    [TOKEN_GREATER]         = {NULL, parse_binary, P_COMP},
    [TOKEN_GREATER_EQUAL]   = {NULL, parse_binary, P_COMP},
    [TOKEN_PAREN_START]     = {parse_group, NULL, P_NONE},
    [TOKEN_PAREN_END]       = {NULL, NULL, P_NONE},
    [TOKEN_CURLY_START]     = {NULL, NULL, P_NONE},
    [TOKEN_CURLY_END]       = {NULL, NULL, P_NONE},
    [TOKEN_AND]             = {NULL, parse_and, P_AND},
    [TOKEN_OR]              = {NULL, parse_or, P_OR},
    [TOKEN_NIL]             = {parse_nil, NULL, P_NONE},
    [TOKEN_VAR]             = {NULL, NULL, P_NONE},
    [TOKEN_FUN]             = {NULL, NULL, P_NONE},
    [TOKEN_CLASS]           = {NULL, NULL, P_NONE},
    [TOKEN_PRINT]           = {NULL, NULL, P_NONE},
    [TOKEN_THIS]            = {NULL, NULL, P_NONE},
    [TOKEN_SUPER]           = {NULL, NULL, P_NONE},
    [TOKEN_FOR]             = {NULL, NULL, P_NONE},
    [TOKEN_IF]              = {NULL, NULL, P_NONE},
    [TOKEN_ELSE]            = {NULL, NULL, P_NONE},
    [TOKEN_WHILE]           = {NULL, NULL, P_NONE},
    [TOKEN_RETURN]          = {NULL, NULL, P_NONE},
    [TOKEN_IDENTIFIER]      = {(void *)parse_var_val, NULL, P_NONE},
    [TOKEN_SEMICOLON]       = {NULL, NULL, P_NONE},
    [TOKEN_COMMA]           = {NULL, NULL, P_NONE},
    [TOKEN_DOT]             = {NULL, NULL, P_NONE},
    [TOKEN_ERROR]           = {NULL, NULL, P_NONE},
    [TOKEN_EOF]             = {NULL, NULL, P_NONE},
};

static Rule* rule_of(TokenType type) {
    return &rules[type];
}

