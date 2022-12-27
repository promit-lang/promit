#include <promit/promit.h>
#include <promit_scanner.h>

// A generic parser for our compiler. Every 'promit_Compiler_compile' call has 
// it's own parser.

typedef struct struct_Parser {
    // The VM.

    SalamanderVM* vm;

    // The tokenizer of our parser.

    Scanner* scanner;

    // Token we just lexed.

    Token previous;

    // Current token we are lexing through.

    Token current;

    // The next token we will lex.

    Token next;

    // The SalamanderVM compiler backend kit.

    CompilerKit* kit;

    // The function to call if an error occurs.
} Parser;

static void error(Parser* parser, Token token, const char* message) {
    // TODO: Print error with config error fn.
}

// Prints an error pointing to the current token.

static void error_at_current(Parser* parser, const char* message) {
    error(parser, parser -> current, message);
}

// Consumes the currently lexed token, if token type matches with provided 
// type. Otherwise, prints the given error message.

static void consume(Parser* parser, TokenType type, const char* errmsg) {
    if(likely(parser -> current.type == type)) {
        advance(parser);

        return;
    }

    error_at_current(parser, errmsg);
}

// Advances one token each.
//     previous = current
//     current  = next
//     next     = New token from scanner.

static void advance(Parser* parser) {
    parser -> previous = parser -> current;
    parser -> current  = parser -> next;

    parser -> next = promit_Scanner_next_token(parser -> scanner);

    // If we find any error in the scanning, ...

    if(parser -> next.type == TOKEN_ERROR) {
        // ...report it if no error is reported previously.

        if(unlikely(parser -> current.type == TOKEN_ERROR)) return;

        error_at_current(parser, parser -> current.start);
    }
}

// Initializes the parser and makes it ready to rock.

static void parser_init(SalamanderVM* vm, Parser* parser, 
    Scanner* scanner, CompilerKit* kit) 
{
    parser -> vm      = vm;
    parser -> scanner = scanner;
    parser -> kit     = kit;

    advance(parser);    // Loads the current token.
    advance(parser);    // Loads the next token.
}

CompilerKit* promit_Compiler_compile(SalamanderVM* vm, const char* source, 
    bool print_errors, PromitErrorFn errfn) 
{
    CompilerKit* kit = salamander_CompilerKit_new(vm);

    // Each compilation call has it's own scanner and parser object.

    Scanner scanner;

    promit_Scanner_init(&scanner, source);

    Parser parser;
    
    parser_init(vm, &parser, &scanner, kit);

    expresssion(&parser);

    consume(&parser, TOKEN_EOF, "Expected an end of expression!");

    return kit;
}