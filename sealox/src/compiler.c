#include <stdio.h>
#include "common.h"
#include "compiler.h"
#include "scanner.h"

void compile(const char* program) {
    init_scanner(program); 

    int line = -1;
    while(true) {
        Token token = scan_token();
        printf("%4d ", token.line); 
        line = token.line;

        print_token_type(token.type);
        printf("'%.*s'\n", token.length, token.start);

        if (token.type == TOKEN_EOF) {
            break;
        }
    }
}
