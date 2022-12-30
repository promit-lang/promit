/** Implements the API functions in 'promit/promit.h'. */

#include <promit/promit.h>
#include <promit_scanner.h>

// A generic parser for our compiler. Every 'promit_Compiler_compile' call has 
// it's own parser.

typedef struct struct_Parser {
    // The source code.

    const char* source;

    // The VM.

    SalamanderVM* vm;

    // The tokenizer of our parser.

    Scanner* scanner;

    // The SalamanderVM compiler backend kit.

    CompilerKit* kit;
    
    // Current Promit compiler configuration.

    PromitConfiguration* config;

    // The sequence of source code lines.

    const char** lines;

    // Token we just lexed.

    Token previous;

    // Current token we are lexing through.

    Token current;

    // The next token we will lex.

    Token next;

    // Total number of lines in the source code.

    int line_count;
} Parser;

// Increases a 32-bit integer number and makes it a power of 2.

static inline int power_of_2(int x) {
    x--;

    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;

    x++;

    return x;
}

// Stores every single line in source code in a line array under parser.

static void linefy(Parser* parser) {
    PromitReallocatorFn _realloc = parser -> config -> reallocator;

    // Flexible capacity.

    int capacity = 8, count = 0;

    // To store the source lines.

    char** lines = (char**) _realloc(NULL, capacity * sizeof(char*));

    char* prev, *current;
    
    prev = current = parser -> source;

    while(*current != '\0') {
        // If we encounter a newline, store the previous line.

        if(*current == '\n') {
            int len = (int) (current - prev);

            // Increase the capacity if overflows.

            if(count + 1 > capacity) {
                capacity = power_of_2(count + 1);

                lines = (char**) _realloc(lines, capacity);
            }

            char* line = (char*) _realloc(NULL, (len + 1) * sizeof(char));

            memcpy(line, prev, len * sizeof(char));

            line[len] = '\0';

            // Store the line.

            lines[count++] = line;

            // Go to next line/character.

            prev = current = current + 1u;
        }
    }

    // Set the stored lines.

    parser -> lines = lines;
}

static void error(Parser* parser, Token token, const char* message) {
    // If we don't have any error function to dump our error to, do nothing.

    if(unlikely(parser -> config -> error == NULL)) 
        return;

    ErrorData data;

    // Fill the data.

    data.message  = message;
    data.line     = parser -> lines[token.line - 1];
    data.column   = token.column;
    data.module   = "dummy";    // TODO: Add module name.
    data.length   = token.length;
    data.line_num = token.line;

    parser -> config -> error(&data);
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

static void parser_init(SalamanderVM* vm, Parser* parser, Scanner* scanner, 
    CompilerKit* kit, const char* source, PromitConfiguration* config) 
{
    promit_Scanner_init(scanner, source);

    parser -> source     = source;
    parser -> vm         = vm;
    parser -> scanner    = scanner;
    parser -> kit        = kit;
    parser -> config     = config;
    parser -> lines      = NULL;
    parser -> line_count = 0;

    linefy(parser);

    advance(parser);    // Loads the current token.
    advance(parser);    // Loads the next token.
}

// Frees the parser.

static void parser_free(Parser* parser) {
    for(int i = 0; i < parser -> line_count; i++) 
        parser -> config -> reallocator(parser -> lines[i], 0u);
    
    parser -> config -> reallocator(parser -> lines, 0u);
}

CompilerKit* promit_Compiler_compile(SalamanderVM* vm, const char* source, 
    bool print_errors, PromitConfiguration* config) 
{
    CompilerKit* kit = salamander_CompilerKit_new(vm);

    // Each compilation call has it's own scanner and parser object.

    Scanner scanner;

    Parser parser;
    
    // Initialize the parser.

    parser_init(vm, &parser, &scanner, kit, source, config);

    expresssion(&parser);

    consume(&parser, TOKEN_EOF, "Expected an end of expression!");

    // Release the parser.

    parser_free(&parser);

    return kit;
}