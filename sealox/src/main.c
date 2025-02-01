#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"

void repl() {
    char line[1024];

    while(true) {
       printf("> "); 
       if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
       }

       interpret(line);
    }
}

char* read_file(const char* file_name) {
    FILE* file = fopen(file_name, "rb");
    if (file == NULL) {
        fprintf(stderr, "Unable to open file \"%s\"\n", file_name);
        exit(1);
    }

    fseek(file, 0L, SEEK_END);
    size_t s = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(s + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\"\n", file_name);
        exit(1);
    }
    size_t b_read = fread(buffer, sizeof(char), s, file);
    if (b_read < s) {
        fprintf(stderr, "Unable to read file \"%s\"\n", file_name);
        exit(1);
    }
    buffer[b_read] = '\0';

    fclose(file);
    return buffer;
}

void run_file(const char* file) {
    char* program = read_file(file); 
    IntrResult result = interpret(program);
    free(program);

    if (result != INTR_OK) {
        exit(1);
    }
}

int main(int argc, const char* argv[]) {
    init_vm();

    if (argc > 1) {
        run_file(argv[1]);
    } else {
        repl();
    }

    free_vm();
    return 0;
}
