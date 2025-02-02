#include <string.h>
#include <stdio.h>
#include "common.h"
#include "scanner.h"

typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

Scanner scanner;

void init_scanner(const char* program) {
    scanner.start = program;
    scanner.current = program;
    scanner.line = 1;
}

bool at_end() {
    return *scanner.current == '\0'; 
}

static char advance() {
    return *scanner.current++;
}

char peek() {
    return *scanner.current;
}

char peek_next() {
    return at_end() ? '\0' : *(scanner.current + 1);
}

bool check(char c) {
   return peek() == c; 
}

bool match(char c) {
   if (check(c)) {
        advance();
        return true;
   }
   return false;
}

Token mk_token(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;

    return token;
}

Token mk_err(const char* msg) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = msg;
    token.length = (int)strlen(msg);
    token.line = scanner.line;

    return token;
}

void skip_whitespace() {
    while(true) {
        char c = peek(); 
        switch(c) {
            case ' ':
            case '\t':
            case '\r':
                advance();
                break;
            case '\n':
                scanner.line++;
                advance();
                break;
            case '/':
                if (peek_next() == '/') {
                    while (peek() != '\n' && !at_end()) {
                        advance(); 
                    }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
   }

}

bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

bool is_alpha(char c) {
    return c >= 'a' && c <= 'z' 
        || c >= 'A' && c <= 'Z'
        || c == '_';
}

Token mk_number() {
    while(is_digit(peek())) {
        advance();
    }
    if (peek() == '.') {
        advance();
        while(is_digit(peek())) {
            advance();
        }
    }
    return mk_token(TOKEN_NUMBER);
}

TokenType check_keyword(int start, int length, char* part, TokenType type) {
    if (scanner.current - scanner.start == start + length 
            && memcmp(scanner.start + start, part, length) == 0) {
        return type; 
    }
    return TOKEN_IDENTIFIER;
}

TokenType mk_keyword_or_id_type() {
    switch(scanner.start[0]) {
        case 'a':
            return check_keyword(1, 2, "nd", TOKEN_AND);
        case 'o':
            return check_keyword(1, 1, "r", TOKEN_OR);
        case 'n':
            return check_keyword(1, 2, "il", TOKEN_NIL);
        case 'v':
            return check_keyword(1, 2, "ar", TOKEN_VAR);
        case 'f':
            printf("f\n");
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'u':
                        return check_keyword(2, 1, "n", TOKEN_FUN);
                    case 'o':
                        return check_keyword(2, 1, "r", TOKEN_FOR);
                    case 'a':
                        return check_keyword(2, 3, "lse", TOKEN_FALSE);
                }
            }
            break;
        case 'c':
            return check_keyword(1, 4, "lass", TOKEN_CLASS);
        case 'p':
            return check_keyword(1, 4, "rint", TOKEN_PRINT);
        case 't':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'h':
                        return check_keyword(2, 2, "is", TOKEN_THIS);
                    case 'r':
                        return check_keyword(2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
        case 's':
            return check_keyword(1, 4, "uper", TOKEN_SUPER);
        case 'i':
            return check_keyword(1, 1, "f", TOKEN_IF);
        case 'e':
            return check_keyword(1, 3, "lse", TOKEN_ELSE);
        case 'w':
            return check_keyword(1, 4, "hile", TOKEN_WHILE);
        case 'r':
            return check_keyword(1, 5, "eturn", TOKEN_RETURN);
    }
    return TOKEN_IDENTIFIER;
}

Token mk_keyword_or_id() {
    while(is_alpha(peek()) || is_digit(peek())) {
        advance();
    }

    return mk_token(mk_keyword_or_id_type());
}

Token mk_str() {
    while(peek() != '"' && !at_end()) {
        advance();
    }
    if (peek() != '"') {
        return mk_err("Unterminated string");
    }
    advance();
    return mk_token(TOKEN_STRING);
}

Token scan_token() {
    skip_whitespace();
    scanner.start = scanner.current; 
    if (at_end()) {
        return mk_token(TOKEN_EOF);
    }

    char c = advance();

    if (is_digit(c)) {
        return mk_number();
    } 

    if(is_alpha(c)) {
        return mk_keyword_or_id();
    }

    switch(c) {
        // single character
        case  '(':
            return mk_token(TOKEN_PAREN_START);
        case ')':
            return mk_token(TOKEN_PAREN_END);
        case  '{':
            return mk_token(TOKEN_CURLY_START);
        case '}':
            return mk_token(TOKEN_CURLY_END);
        case '+':
            return mk_token(TOKEN_PLUS);
        case '-':
            return mk_token(TOKEN_MINUS);
        case '*':
            return mk_token(TOKEN_STAR);
        case ';':
            return mk_token(TOKEN_SEMICOLON);
        case ',':
            return mk_token(TOKEN_COMMA);
        case '.':
            return mk_token(TOKEN_DOT);
        case '/': 
            return mk_token(TOKEN_SLASH);
        // two characters
        case '!':
            return mk_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return mk_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<':
            return mk_token(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return mk_token(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '"':
            return mk_str();
    }

   return mk_err("Unexpected character");
}

#define PRINT_PAD(s) printf("%-20s", s)
#define PRINT_TOKEN(type) PRINT_PAD(#type)

void print_token_type(TokenType type) {
    switch(type) {
        case TOKEN_NUMBER: PRINT_TOKEN(TOKEN_NUMBER); break;
        case TOKEN_STRING: PRINT_TOKEN(TOKEN_STRING); break;
        case TOKEN_TRUE: PRINT_TOKEN(TOKEN_TRUE); break;
        case TOKEN_FALSE: PRINT_TOKEN(TOKEN_FALSE); break;
        case TOKEN_PLUS: PRINT_TOKEN(TOKEN_PLUS); break;
        case TOKEN_MINUS: PRINT_TOKEN(TOKEN_MINUS); break;
        case TOKEN_STAR: PRINT_TOKEN(TOKEN_STAR); break;
        case TOKEN_SLASH: PRINT_TOKEN(TOKEN_SLASH); break;
        case TOKEN_BANG: PRINT_TOKEN(TOKEN_BANG); break;
        case TOKEN_EQUAL: PRINT_TOKEN(TOKEN_EQUAL); break;
        case TOKEN_EQUAL_EQUAL: PRINT_TOKEN(TOKEN_EQUAL_EQUAL); break;
        case TOKEN_BANG_EQUAL: PRINT_TOKEN(TOKEN_BANG_EQUAL); break;
        case TOKEN_LESS: PRINT_TOKEN(TOKEN_LESS); break;
        case TOKEN_LESS_EQUAL: PRINT_TOKEN(TOKEN_LESS_EQUAL); break;
        case TOKEN_GREATER: PRINT_TOKEN(TOKEN_GREATER); break;
        case TOKEN_GREATER_EQUAL: PRINT_TOKEN(TOKEN_GREATER_EQUAL); break;
        case TOKEN_PAREN_START: PRINT_TOKEN(TOKEN_PAREN_START); break;
        case TOKEN_PAREN_END: PRINT_TOKEN(TOKEN_PAREN_END); break;
        case TOKEN_CURLY_START: PRINT_TOKEN(TOKEN_CURLY_START); break;
        case TOKEN_CURLY_END: PRINT_TOKEN(TOKEN_CURLY_END); break;
        case TOKEN_AND: PRINT_TOKEN(TOKEN_AND); break;
        case TOKEN_OR: PRINT_TOKEN(TOKEN_OR); break;
        case TOKEN_NIL: PRINT_TOKEN(TOKEN_NIL); break;
        case TOKEN_VAR: PRINT_TOKEN(TOKEN_VAR); break;
        case TOKEN_FUN: PRINT_TOKEN(TOKEN_FUN); break;
        case TOKEN_CLASS: PRINT_TOKEN(TOKEN_CLASS); break;
        case TOKEN_PRINT: PRINT_TOKEN(TOKEN_PRINT); break;
        case TOKEN_THIS: PRINT_TOKEN(TOKEN_THIS); break;
        case TOKEN_SUPER: PRINT_TOKEN(TOKEN_SUPER); break;
        case TOKEN_FOR: PRINT_TOKEN(TOKEN_FOR); break;
        case TOKEN_IF: PRINT_TOKEN(TOKEN_IF); break;
        case TOKEN_ELSE: PRINT_TOKEN(TOKEN_ELSE); break;
        case TOKEN_WHILE: PRINT_TOKEN(TOKEN_WHILE); break;
        case TOKEN_RETURN: PRINT_TOKEN(TOKEN_RETURN); break;
        case TOKEN_IDENTIFIER: PRINT_TOKEN(TOKEN_IDENTIFIER); break;
        case TOKEN_SEMICOLON: PRINT_TOKEN(TOKEN_SEMICOLON); break;
        case TOKEN_COMMA: PRINT_TOKEN(TOKEN_COMMA); break;
        case TOKEN_DOT: PRINT_TOKEN(TOKEN_DOT); break;
        case TOKEN_ERROR: PRINT_TOKEN(TOKEN_ERROR); break;
        case TOKEN_EOF: PRINT_TOKEN(TOKEN_EOF); break;
        default: PRINT_PAD("UNKNOWN"); break;
    }
}
