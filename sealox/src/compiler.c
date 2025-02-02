#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "compiler.h"
#include "scanner.h"
#ifdef DEBUG_COMP
#include "dev.h"
#endif

typedef struct {
    Token curr;
    Token prev;
    bool err;
    bool panic;
} Parser;

Parser parser;
Ops* ops_ptr;

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
static Rule* rule_of(TokenType type);
static void parse_prec(Prec prec);

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

Ops* curr_ops() {
    return ops_ptr;
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
    fn();

    while (prec <= rule_of(parser.curr.type)->prec) {
        advance();
        Pfn ifn = rule_of(parser.prev.type)->infix;
        ifn();
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

bool compile(const char* program, Ops* ops) {
    init_scanner(program); 
    ops_ptr = ops;

    parser.err = false;
    parser.panic = false;

    advance();
    parse_expr();
    consume(TOKEN_EOF, "Expected EOF");
    end_comp();
    return !parser.err;
}

// mapping from tokens to rules
Rule rules[] = {
    [TOKEN_NUMBER]          = {parse_num, NULL, P_NONE},
    [TOKEN_STRING]          = {NULL, NULL, P_NONE},
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
    [TOKEN_AND]             = {NULL, NULL, P_NONE},
    [TOKEN_OR]              = {NULL, NULL, P_NONE},
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
    [TOKEN_IDENTIFIER]      = {NULL, NULL, P_NONE},
    [TOKEN_SEMICOLON]       = {NULL, NULL, P_NONE},
    [TOKEN_COMMA]           = {NULL, NULL, P_NONE},
    [TOKEN_DOT]             = {NULL, NULL, P_NONE},
    [TOKEN_ERROR]           = {NULL, NULL, P_NONE},
    [TOKEN_EOF]             = {NULL, NULL, P_NONE},
};

static Rule* rule_of(TokenType type) {
    return &rules[type];
}

