#include <promit/promit.h>
#include <promit_scanner.h>

#include <stdio.h>

ObjFn* promit_Compiler_compile(PromitVM* vm, const char* source, 
    bool print_errors) 
{
    Scanner scanner;

    promit_Scanner_init(&scanner, source);

    int line = -1;

    Token token;

    while(true) {
        token = promit_Scanner_next_token(&scanner);

        if(token.line != line) {
            printf("%4d ", line);

            line = token.line;
        } else printf("   | ");

        printf("%2d %.*s", token.type, token.length, token.start);

        if(token.type == TOKEN_EOF) 
            break;
    }

    return NULL;
}