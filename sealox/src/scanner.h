#ifndef scanner_h
#define scanner_h

typedef enum {
    // literals
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_TRUE,
    TOKEN_FALSE,
    // operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_BANG,
    TOKEN_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_BANG_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    // groups
    TOKEN_PAREN_START,
    TOKEN_PAREN_END,
    TOKEN_CURLY_START,
    TOKEN_CURLY_END,
    // keywords
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NIL,
    TOKEN_VAR,
    TOKEN_FUN,
    TOKEN_CLASS,
    TOKEN_PRINT,
    TOKEN_THIS,
    TOKEN_SUPER,
    // keywords - control flow
    TOKEN_FOR,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_RETURN,
    // special
    TOKEN_IDENTIFIER,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_ERROR,
    TOKEN_EOF,
} TokenType;

typedef struct {
    TokenType  type;
    const char* start;
    int length;
    int line;
} Token;

void init_scanner(const char* program);
Token scan_token();
void print_token_type(TokenType type);

#endif
