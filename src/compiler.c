#include <promit/promit.h>
#include <promit_scanner.h>

// A generic parser for our compiler. Every 'promit_Compiler_compile' call has 
// it's own parser.

typedef struct struct_Parser {
    // The tokenizer of our parser.

    Scanner* scanner;

    // Token we just lexed.

    Token previous;

    // Current token we are lexing through.

    Token current;

    // The next token we will lex.

    Token next;   
} Parser;

static void advance(Parser* parser) {
    parser -> previous = parser -> current;
    parser -> current = parser -> next;

    parser -> next = promit_Scanner_next_token(parser -> scanner);

    // TODO: Handle TOKEN_ERROR.
}

// Initializes the parser and makes it ready to rock.

static void parser_init(Parser* parser, Scanner* scanner) {
    parser -> scanner = scanner;

    advance(parser);    // Loads the current token.
    advance(parser);    // Loads the next token.
}

ObjFn* promit_Compiler_compile(SalamanderVM* vm, const char* source, 
    bool print_errors) 
{
    // Each compilation call has it's own scanner and parser object.

    Scanner scanner;

    promit_Scanner_init(&scanner, source);

    Parser parser;
    
    parser_init(&parser, &scanner);

    expresssion(&parser);

    return NULL;
}