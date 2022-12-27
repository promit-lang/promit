#include <promit_scanner.h>

// Standard C includes.

#include <string.h>
#include <errno.h>
#include <stdlib.h>

// Some macros, which shortifies some frequently used functions.

#define MAKE_TOKEN(type) return make_token(scanner, type)
#define MATCH(expected)  match(scanner, expected)
#define ADVANCE()        advance(scanner)
#define PEEK()           peek(scanner)
#define PEEK2()          peek_next(scanner)
#define ATEND()          is_at_end(scanner)

// To define the number type of number string to transform to number.
// 
// Promit supports 3 types of number literal.

typedef enum enum_NumberType {
    NUMBER_TYPE_BINARY,
    NUMBER_TYPE_DECIMAL,
    NUMBER_TYPE_HEXADECIMAL
} NumberType;

// Creates a token of provided token type.

static Token make_token(Scanner* scanner, TokenType type) {
    Token token;

    token.type   = type;
    token.line   = scanner -> line;
    token.start  = scanner -> start;
    token.length = (int) (scanner -> current - scanner -> start);
    token.value  = 0;

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
    token.value  = 0;

    return token;
}

static bool is_at_end(Scanner* scanner) {
    // If the current reading character is termination character, we are at 
    // the end of the source buffer.

    return *scanner -> current == '\0';
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
    if(unlikely(ATEND())) 
        return false;
    
    if(*scanner -> current != expected) 
        return false;
    
    scanner -> current++;

    return true;
}

static Token two_char_token(Scanner* scanner, char next, 
    TokenType type_if, TokenType type_else) 
{
    if(MATCH(next)) 
        MAKE_TOKEN(type_if);
    
    MAKE_TOKEN(type_else);
}

// Peeks at the current lexed character without advancing the scanner.

static char peek(Scanner* scanner) {
    return *scanner -> current;
}

// Peeks at the next character respecting to current lexed character.

static char peek_next(Scanner* scanner) {
    if(ATEND()) 
        return '\0';
    
    return scanner -> current[1];
}

// Skips all the whitespaces, e.g. spaces, tabs etc.

static void skip_whitespace(Scanner* scanner) {
    char ch;

    while(true) {
        ch = PEEK();

        switch(ch) {
            case ' ':
            case '\t': 
            case '\r': 
                ADVANCE();
                break;
            
            case '\n': {
                scanner -> line++;

                ADVANCE();
                
                break;
            }

            // A variant of single line comment in Promit.

            case '#': {
                while(!ATEND() && ADVANCE() != '\n') /** No statement. */;

                scanner -> line++;
                
                break;
            }

            case '/': {
                // For single line comments.

                if(PEEK2() == '/') {
                    while(!ATEND() && ADVANCE() != '\n') /** No statement. */ ;

                    scanner -> line++;

                    break;
                }

                // For multiline/block comments.

                else if(PEEK2() == '*') {
                    while(!ATEND()) {
                        if(ADVANCE() == '*' && PEEK() == '/') {
                            ADVANCE();

                            break;
                        }
                        else if(likely(PEEK() == '\n'))
                            scanner -> line++;
                    }

                    break;
                }

                return;
            }

            default: return;
        }
    }
}

// Reads the whole string upto the closing quote/apostrophe.
// 
// Promit supports multiline string.

static Token read_string(Scanner* scanner, char closing) {
    // Ignore the quote/apostrophe.

    scanner -> start = scanner -> current;

    while(!ATEND() && PEEK() != closing) {
        if(unlikely(PEEK() == '\n')) 
            scanner -> line++;
        
        ADVANCE();
    }

    if(unlikely(ATEND())) return error_token(scanner, "Unterminated string!");

    Token token = make_token(scanner, TOKEN_STRING);

    // The closing quote/apostrophe.

    ADVANCE();

    return token;
}

// Checks whether a character is a digit character.

static bool is_digit(char ch) {
    return ch >= '0' && ch <= '9';
}

// Emits a number token.

static Token make_number(Scanner* scanner, NumberType type) {
    // Reset the error message number <errno.h>.

    errno = 0;

    double num = -1;

    switch(type) {
        case NUMBER_TYPE_DECIMAL: 
            num = strtod(scanner -> start, NULL);
            break;
        
        case NUMBER_TYPE_HEXADECIMAL: 
            num = (double) strtoll(scanner -> start, NULL, 16);
            break;
        
        case NUMBER_TYPE_BINARY: 
            num = (double) strtoll(scanner -> start, NULL, 2);
            break;
        
        default: UNREACHABLE();
    }

    if(errno == ERANGE) 
        return error_token(scanner, 
            "Number literal was too large to be converted. "
            "Make sure it fits within 64-bit integer.");

    Token token = make_token(scanner, TOKEN_NUMBER);

    token.value = num;

    return token;
}

// Lexes through a decimal number.

static Token read_number(Scanner* scanner) {
    // Read through all the leading digits.

    while(is_digit(PEEK())) 
        ADVANCE();
    
    if(likely(MATCH('.'))) {
        // Consume all the digits after that.

        while(is_digit(PEEK())) 
            ADVANCE();
    }

    if(MATCH('e') || MATCH('E')) {
        if(MATCH('+')) MATCH('-');

        if(unlikely(!is_digit(PEEK())))  
            return error_token(scanner, "Unterminated scientific notation!");
        
        // Consume the exponential digits.

        while(is_digit(PEEK())) 
            ADVANCE();
    }

    return make_number(scanner, NUMBER_TYPE_DECIMAL);
}

// Lexes through a hexadecimal number.

static Token read_hex_number(Scanner* scanner) {
    char ch = PEEK();

    while((ch >= '0' && ch <= '9') || 
        (ch >= 'a' && ch <= 'f') || 
        (ch >= 'A' && ch <= 'F')) 
    {
        ADVANCE();

        ch = PEEK();
    }

    return make_number(scanner, NUMBER_TYPE_HEXADECIMAL);
}

// Lexes through a binary number.

static Token read_bin_number(Scanner* scanner) {
    char ch = PEEK();

    while(ch >= '0' && ch <= '1') {
        ADVANCE();

        ch = PEEK();
    }

    return make_number(scanner, NUMBER_TYPE_BINARY);
}

// Check whether provided character is an alphabetical character.

static bool is_alpha(char ch) {
    // 'thi$' is a valid variable name.

    return (ch >= 'a' && ch <= 'z') || 
           (ch >= 'A' && ch <= 'Z') || 
           (ch == '$' || ch == '_');
}

// Matches rest of the characters with currently lexed token striding from 
// character and returns the provided token type if the character maches.

static TokenType check_keyword(Scanner* scanner, int start, 
    int length, const char* rest, TokenType type) 
{
    // Match the length first.

    if((scanner -> current - scanner -> start == start + length) && 
           memcmp(scanner -> start + start, rest, length * sizeof(char)) == 0) 
    {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

static TokenType identifier_type(Scanner* scanner) {
#define CHECK(start, length, rest, type)                       \
    return check_keyword(scanner, start, length, rest, type)

    switch(scanner -> start[0]) {
        case 'b': CHECK(1, 3, "ase", TOKEN_BASE);
        case 'c': 
            if(likely(scanner -> current - scanner -> start > 1)) {
                switch(scanner -> start[1]) {
                    case 'a': 
                        if(scanner -> current - scanner -> start > 2) {
                            switch(scanner -> start[2]) {
                                case 's': CHECK(3, 1, "e", TOKEN_CASE);
                                case 't': CHECK(3, 4, "alog", TOKEN_CATALOG);
                            }
                        }

                        break;

                    case 'l': CHECK(2, 3, "ass", TOKEN_CLASS);
                    case 'o': CHECK(2, 6, "ntinue", TOKEN_CONTINUE);
                }
            }

            break;
        
        case 'd': 
            if(likely(scanner -> current - scanner -> start > 1)) {
                switch(scanner -> start[1]) {
                    case 'e': CHECK(2, 1, "l", TOKEN_DEL);
                    case 'o': return TOKEN_DO;
                }
            }

            break;
        
        case 'e': 
            if(likely(scanner -> current - scanner -> start > 1)) {
                switch(scanner -> start[1]) {
                    case 'l': CHECK(2, 2, "se", TOKEN_ELSE);
                    case 'n': CHECK(2, 2, "um", TOKEN_ENUM);
                    case 'x': CHECK(2, 4, "cept", TOKEN_EXCEPT);
                }
            }

            break;
        
        case 'f': 
            if(likely(scanner -> current - scanner -> start > 1)) {
                switch(scanner -> start[1]) {
                    case 'a': CHECK(2, 3, "lse", TOKEN_FALSE);
                    case 'i': 
                        if(likely(scanner -> current - scanner -> start > 2)) {
                            switch(scanner -> start[2]) {
                                case 'b': CHECK(3, 2, "er", TOKEN_FIBER);
                                case 'n': CHECK(3, 2, "al", TOKEN_FINAL);
                            }
                        }

                        break;
                    
                    case 'n': return TOKEN_FN;
                    case 'o': CHECK(2, 1, "r", TOKEN_FOR);
                }
            }

            break;
        
        case 'g': CHECK(1, 5, "etter", TOKEN_GETTER);
        case 'i': 
            if(likely(scanner -> current - scanner -> start > 1)) {
                switch(scanner -> start[1]) {
                    case 'f': return TOKEN_IF;
                    case 'n': 
                        if(likely(scanner -> current - scanner -> start > 2)) {
                            switch(scanner -> start[2]) {
                                case 'f': CHECK(3, 5, "inity", TOKEN_INFINITY);
                                case 's': CHECK(3, 3, "tof", TOKEN_INSTOF);
                            }
                        }

                        break;
                }
            }

            break;

        case 'm': CHECK(1, 4, "atch", TOKEN_MATCH);
        case 'n': 
            if(unlikely(scanner -> current - scanner -> start > 1)) {
                switch(scanner -> start[1]) {
                    case 'a': CHECK(2, 1, "n", TOKEN_NAN);
                    case 'e': CHECK(2, 2, "xt", TOKEN_NEXT);
                    case 'u': CHECK(2, 2, "ll", TOKEN_NULL);
                }
            }

            break;
        
        case 'p': 
            if(likely(scanner -> current - scanner -> start > 1)) {
                switch(scanner -> start[1]) {
                    case 'a': CHECK(2, 4, "rent", TOKEN_PARENT);
                    case 'r': 
                        if(scanner -> current - scanner -> start > 2) {
                            switch(scanner -> start[2]) {
                                case 'e': CHECK(3, 5, "vious", TOKEN_PREVIOUS);
                                case 'i': CHECK(3, 4, "vate", TOKEN_PRIVATE);
                                case 'o': CHECK(3, 6, "tected", TOKEN_PROTECTED);
                            }
                        }

                        break;
                    
                    case 'u': CHECK(2, 4, "blic", TOKEN_PUBLIC);
                }
            }

            break;
        
        case 'r': CHECK(1, 5, "eturn", TOKEN_RETURN);
        case 's': 
            if(likely(scanner -> current - scanner -> start > 1)) {
                switch(scanner -> start[1]) {
                    case 'e': 
                        if(scanner -> current - scanner -> start > 2) {
                            switch(scanner -> start[2]) {
                                case 'l': CHECK(3, 1, "f", TOKEN_SELF);
                                case 't': CHECK(3, 3, "ter", TOKEN_SETTER);
                            }
                        }

                        break;
                    
                    case 't': CHECK(2, 4, "atic", TOKEN_STATIC);
                }
            }

            break;
        
        case 't': 
            if(likely(scanner -> current - scanner -> start > 1)) {
                switch(scanner -> start[1]) {
                    case 'a': CHECK(2, 2, "ke", TOKEN_TAKE);
                    case 'r': CHECK(2, 2, "ue", TOKEN_TRUE);
                }
            }

            break;
        
        case 'w': CHECK(1, 4, "hile", TOKEN_WHILE);
        case 'y': CHECK(1, 4, "ield", TOKEN_YIELD);
    }

    return TOKEN_IDENTIFIER;

#undef CHECK
}

// Lexes through the whole identifier.

static Token read_identifier(Scanner* scanner) {
    // There can be also numbers in identifiers.
    // 
    //     th123 -> Valid identifier.
    //     123th -> Not valid identifier.
    // 
    // We are sure the identifier we are lexing is valid. Cause the first alpha
    // character has already been consumed by 'promit_Scanner_next_token'.

    while(is_alpha(PEEK()) || is_digit(PEEK())) 
        ADVANCE();
    
    // Now we have consumes all the alpha character there is, we will figure 
    // out what type of identifier it is. For example, 
    // 
    //     false -> Literal type identifier.
    //     some_box -> Variable type identifier.

    MAKE_TOKEN(identifier_type(scanner));
}

// void promit_Scanner_init(Scanner*, const char* const);
// 
// Initialize the scanner.

void promit_Scanner_init(Scanner* scanner, const char* source) {
    scanner -> source  = source;
    scanner -> start   = source;
    scanner -> current = source;
    scanner -> line    = 1;

    linefy(scanner);
}

// void promit_Scanner_next_token(Scanner*);
// 
// Scan and emit the next token whenever the compiler needs it.

Token promit_Scanner_next_token(Scanner* scanner) {
#define MAKE_BI_TOKEN(next, type_if, type_else)                  \
    return two_char_token(scanner, next, type_if, type_else)

    // We are gonna discard whitespaces between tokens.

    skip_whitespace(scanner);

    scanner -> start = scanner -> current;

    // If we are at the end of source buffer, return TOKEN_EOF.

    if(unlikely(ATEND())) 
        MAKE_TOKEN(TOKEN_EOF);
    
    // Get the current character.

    char c = ADVANCE();

    // Literal.

    // Read a Hexadecimal number if it starts with '0x'.

    if(c == '0' && PEEK() == 'x') {
        ADVANCE();    // Consume 'x'.

        return read_hex_number(scanner);
    }
    
    // Read a binary number if it starts with '0b'.

    if(c == '0' && PEEK() == 'b') {
        ADVANCE();    // Consume 'b'.

        return read_bin_number(scanner);
    }

    // Else try reading it like a decimal number.

    if(likely(is_digit(c))) return read_number(scanner);

    // Read it as an identifier, if it starts with an alphabetical character.

    if(is_alpha(c)) return read_identifier(scanner);


    switch(c) {
        case '(': MAKE_TOKEN(TOKEN_LEFT_PAREN);
        case ')': MAKE_TOKEN(TOKEN_RIGHT_PAREN);
        case '{': MAKE_TOKEN(TOKEN_LEFT_BRACE);
        case '}': MAKE_TOKEN(TOKEN_RIGHT_BRACE);
        case '[': MAKE_TOKEN(TOKEN_LEFT_BRACKET);
        case ']': MAKE_TOKEN(TOKEN_RIGHT_BRACKET);

        case '<': {
            if(likely(MATCH('='))) 
                MAKE_TOKEN(TOKEN_LEFT_ANGLE_EQUAL);
            else if(unlikely(MATCH('<'))) 
                MAKE_TOKEN(TOKEN_LEFT_2ANGLE);
            
            MAKE_TOKEN(TOKEN_LEFT_ANGLE);
        }

        case '>': {
            if(likely(MATCH('='))) 
                MAKE_TOKEN(TOKEN_RIGHT_ANGLE_EQUAL);
            else if(unlikely(MATCH('>'))) 
                MAKE_TOKEN(TOKEN_RIGHT_2ANGLE);
            
            MAKE_TOKEN(TOKEN_RIGHT_ANGLE);
        }

        case '+': {
            if(unlikely(MATCH('='))) 
                MAKE_TOKEN(TOKEN_PLUS_EQUAL);
            else if(likely(MATCH('+'))) 
                MAKE_TOKEN(TOKEN_2PLUS);
            
            MAKE_TOKEN(TOKEN_PLUS);
        }

        case '-': {
            if(unlikely(MATCH('='))) 
                MAKE_TOKEN(TOKEN_MINUS_EQUAL);
            else if(likely(MATCH('-'))) 
                MAKE_TOKEN(TOKEN_2MINUS);
            else if(unlikely(MATCH('>'))) 
                MAKE_TOKEN(TOKEN_ARROW);
            
            MAKE_TOKEN(TOKEN_MINUS);
        }

        case '!': {
            if(likely(MATCH('='))) 
                MAKE_TOKEN(TOKEN_BANG_EQUAL);
            else if(unlikely(MATCH('&'))) 
                MAKE_BI_TOKEN('=', TOKEN_BANG_AMPERSAND_EQUAL, 
                TOKEN_BANG_AMPERSAND);
            else if(unlikely(MATCH('|'))) 
                MAKE_BI_TOKEN('=', TOKEN_BANG_PIPE_EQUAL, TOKEN_BANG_PIPE);
            
            MAKE_TOKEN(TOKEN_BANG);
        }

        case '/': MAKE_BI_TOKEN('=', TOKEN_SLASH_EQUAL, TOKEN_SLASH);
        case '*': MAKE_BI_TOKEN('=', TOKEN_ASTERISK_EQUAL, TOKEN_ASTERISK);
        case '%': MAKE_BI_TOKEN('=', TOKEN_PERCENT_EQUAL, TOKEN_PERCENT);

        case '&': {
            if(unlikely(MATCH('='))) 
                MAKE_TOKEN(TOKEN_AMPERSAND_EQUAL);
            else if(likely(MATCH('&'))) 
                MAKE_TOKEN(TOKEN_2AMPERSAND);
            
            MAKE_TOKEN(TOKEN_AMPERSAND);
        }

        case '~': MAKE_TOKEN(TOKEN_TILDE);
        case '^': MAKE_BI_TOKEN('=', TOKEN_CARET_EQUAL, TOKEN_CARET);

        case '|': {
            if(unlikely(MATCH('='))) 
                MAKE_TOKEN(TOKEN_PIPE_EQUAL);
            else if(likely(MATCH('|'))) 
                MAKE_TOKEN(TOKEN_2PIPE);
            
            MAKE_TOKEN(TOKEN_PIPE);
        }

        case '\\': MAKE_TOKEN(TOKEN_BACKSLASH);
        case '.': MAKE_TOKEN(TOKEN_PERIOD);
        case '=': MAKE_BI_TOKEN('=', TOKEN_2EQUAL, TOKEN_EQUAL);
        case '?': MAKE_TOKEN(TOKEN_QUESTION);
        case ';': MAKE_TOKEN(TOKEN_SEMICOLON);
        case ':': MAKE_BI_TOKEN(':', TOKEN_2COLON, TOKEN_COLON);
        case ',': MAKE_TOKEN(TOKEN_COMMA);
        case '\'': return read_string(scanner, '\'');
        case '"':  return read_string(scanner, '"');
    }
    
    return unexpected_character(scanner);

#undef MAKE_BI_TOKEN
}

#undef MAKE_TOKEN
#undef MATCH
#undef ADVANCE
#undef PEEK
#undef PEEK2
#undef ATEND