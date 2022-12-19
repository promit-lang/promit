#include <promit_scanner.h>

// Standard C includes.

#include <string.h>

// void promit_Scanner_init(Scanner*, const char* const);
// 
// Initialize the scanner.

void promit_Scanner_init(Scanner* scanner, const char* source) {
    scanner -> source  = source;
    scanner -> start   = source;
    scanner -> current = source;
    scanner -> line    = 1;
}

// Creates a token of provided token type.

static Token make_token(Scanner* scanner, TokenType type) {
    Token token;

    token.type   = type;
    token.line   = scanner -> line;
    token.start  = scanner -> start;
    token.length = (int) (scanner -> current - scanner -> start);
    token.value  = NULL;

    return token;
}

// Creates a token with type 'TOKEN_ERROR', which holds the provided error 
// message.

static Token error_token(Scanner* scanner, const char* message) {
    Token token;

    token.type   = TOKEN_ERROR;
    token.line   = scanner -> line;
    token.start  = message;
    token.length = (int) strlen(message);
    token.value  = NULL;

    return token;
}

static bool is_at_end(Scanner* scanner) {
    // If the current reading character is termination character, we are at 
    // the end of the source buffer.

    return scanner -> current == '\0';
}

// Returns an unexpected character error.

static Token unexpected_character(Scanner* scanner) {
    char buffer[26];

    memcpy(buffer, "Unexpected character '", 22u * sizeof(char));
    memcpy(buffer + 22u, scanner -> current, sizeof(char));
    memcpy(buffer + 23u, "'!", 2u * sizeof(char));

    buffer[25] = '\0';

    return error_token(scanner, buffer);
}

// Advances one character.

static char advance(Scanner* scanner) {
    return *scanner -> current++;
}

// Check whether provided character is current character or not. If it is, 
// advances the character.

static bool match(Scanner* scanner, char expected) {
    if(is_true(is_at_end(scanner) || *scanner -> current != expected)) 
        return false;
    
    scanner -> current++;

    return true;
}

static Token two_char_token(Scanner* scanner, char next, 
    TokenType type_if, TokenType type_else) 
{
    if(is_true(match(scanner, next))) 
        return make_token(scanner, type_if);
    
    return make_token(scanner, type_else);
}

// void promit_Scanner_next_token(Scanner*);
// 
// Scan and emit the next token whenever the compiler needs it.

Token promit_Scanner_next_token(Scanner* scanner) {
#define MAKE_TOKEN(type) return make_token(scanner, type); break;
#define MAKE_BI_TOKEN(next, type_if, type_else)                      \
    return two_char_token(scanner, next, type_if, type_else); break;

    // We are gonna discard whitespaces between tokens.

    skip_whitespaces();

    scanner -> start = scanner -> current;

    // If we are at the end of source buffer, return TOKEN_EOF.

    if(is_true(is_at_end(scanner))) 
        return make_token(scanner, TOKEN_EOF);
    
    // Get the current character.

    char c = advance(scanner);

    switch(c) {
        // Braces.

        case '(': MAKE_TOKEN(TOKEN_LEFT_PAREN);
        case ')': MAKE_TOKEN(TOKEN_RIGHT_PAREN);
        case '{': MAKE_TOKEN(TOKEN_LEFT_BRACE);
        case '}': MAKE_TOKEN(TOKEN_RIGHT_BRACE);
        case '[': MAKE_TOKEN(TOKEN_LEFT_BRACKET);
        case ']': MAKE_TOKEN(TOKEN_RIGHT_BRACKET);
        case '<': MAKE_BI_TOKEN('=', TOKEN_LEFT_ANGLE_EQUAL, TOKEN_LEFT_ANGLE);
        case '>': 
            MAKE_BI_TOKEN('=', TOKEN_RIGHT_ANGLE_EQUAL, TOKEN_RIGHT_ANGLE);

        // Operators.

        case '+': MAKE_TOKEN(TOKEN_PLUS);
        case '-': MAKE_TOKEN(TOKEN_MINUS);
        case '!': MAKE_TOKEN(TOKEN_BANG);
        case '/': MAKE_TOKEN(TOKEN_SLASH);
        case '*': MAKE_TOKEN(TOKEN_ASTERISK);
        case '%': MAKE_TOKEN(TOKEN_PERCENT);
        case '&': MAKE_TOKEN(TOKEN_AMPERSAND);
        case '~': MAKE_TOKEN(TOKEN_TILDE);
        case '^': MAKE_TOKEN(TOKEN_CARET);
        case '|': MAKE_BI_TOKEN('|', TOKEN_2PIPE, TOKEN_PIPE);
        case '\\': MAKE_TOKEN(TOKEN_BACKSLASH);

        // Miscellaneous.

        case '.': MAKE_TOKEN(TOKEN_PERIOD);
        case '=': MAKE_BI_TOKEN('=', TOKEN_2EQUAL, TOKEN_EQUAL);
        case '?': MAKE_TOKEN(TOKEN_QUESTION);
        case '\'': MAKE_TOKEN(TOKEN_APOSTROPHE);
        case '"': MAKE_TOKEN(TOKEN_QUOTE);
        case ';': MAKE_TOKEN(TOKEN_SEMICOLON);
        case ':': MAKE_TOKEN(TOKEN_COLON);
        case ',': MAKE_TOKEN(TOKEN_COMMA);
    }
    
    return unexpected_character(scanner);

#undef MAKE_TOKEN
#undef MAKE_BI_TOKEN
}