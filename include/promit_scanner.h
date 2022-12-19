/**
 * promit_scanner.h
 * 
 * See the 'LICENSE' file for this file's license.
 *
 * This header file and it's respective C translation implements the Scanner, 
 * the main front-end of a compiler. A Scanner basically parses the provided 
 * source code and generates tokens, which makes parsing the expressions and 
 * generating code much easier in the back-end.
 *
 * @authors:
 *   1. SD Asif Hossein <s.dah.ingularity@gmail.com> 17th Dec. 2022
 */

#ifndef __PROMIT_SCANNER_H__
#define __PROMIT_SCANNER_H__

#include <promit/promit.h>
#include <promit_core.h>

// Types of token we are going to generate from the source file.

typedef enum enum_TokenType {
    /** Single character tokens. */

    // Braces.

    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_LEFT_ANGLE,
    TOKEN_RIGHT_ANGLE,

    // Oprators.

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_BANG,
    TOKEN_SLASH,
    TOKEN_ASTERISK,
    TOKEN_PERCENT,
    TOKEN_AMPERSAND,
    TOKEN_TILDE,
    TOKEN_CARET,
    TOKEN_PIPE,
    TOKEN_BACKSLASH,
    
    // Miscellaneous.

    TOKEN_PERIOD,
    TOKEN_EQUAL,
    TOKEN_QUESTION,
    TOKEN_APOSTROPHE,
    TOKEN_QUOTE,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_COMMA,

    /** Bi-character tokens. */

    // Operators.

    TOKEN_2EQUAL,
    TOKEN_2PIPE,
    TOKEN_2AMPERSAND,
    TOKEN_LEFT_2ANGLE,
    TOKEN_RIGHT_2ANGLE,
    TOKEN_BANG_EQUAL,
    TOKEN_LEFT_ANGLE_EQUAL,
    TOKEN_RIGHT_ANGLE_EQUAL,

    // Not-bitwise operators, e.g. bitwise NAND.

    TOKEN_BANG_AMPERSAND,    // !&
    TOKEN_BANG_PIPE,         // !|

    // Miscellaneous.

    TOKEN_2PLUS,
    TOKEN_2MINUS,
    TOKEN_PLUS_EQUAL,
    TOKEN_MINUS_EQUAL,
    TOKEN_SLASH_EQUAL,
    TOKEN_ASTERISK_EQUAL,
    TOKEN_PERCENT_EQUAL,
    TOKEN_PIPE_EQUAL,
    TOKEN_AMPERSAND_EQUAL,
    TOKEN_CARET_EQUAL,
    TOKEN_COLON_COLON,

    // For lambda function expressions and misc.

    TOKEN_ARROW,
    
    /** Multi-character tokens. */

    // For Not-bitwise operation and assignment.
    // e.g. x !&= 5; -> x = x nand 5;

    TOKEN_BANG_AMPERSAND_EQUAL,
    TOKEN_BANG_PIPE_EQUAL,

    /** Keywords. */

    // Variables.

    TOKEN_TAKE,
    TOKEN_FINAL,

    // Class and OOP.

    TOKEN_CLASS,
    TOKEN_GETTER,
    TOKEN_SETTER,
    TOKEN_PUBLIC,
    TOKEN_PRIVATE,
    TOKEN_PROTECTED,
    TOKEN_STATIC,
    TOKEN_SELF,
    TOKEN_PARENT,
    TOKEN_INSTOF,

    // 'catalog' -> equivalent to Java's 'interface'.

    TOKEN_CATALOG,

    // Fibers and functions.

    TOKEN_FIBER,
    TOKEN_FN,
    TOKEN_RETURN,
    TOKEN_YIELD,

    // 'match' -> equivalent to 'switch' in C like languages.

    TOKEN_MATCH,
    TOKEN_NEXT,
    TOKEN_CASE,
    TOKEN_PREVIOUS,
    TOKEN_EXCEPTION,
    
    // Common in loops and match.
    
    TOKEN_BREAK,

    // Loops.

    TOKEN_FOR,
    TOKEN_WHILE,
    TOKEN_DO,
    TOKEN_CONTINUE,

    // For special type enumeration loop. For example,
    // enum(take key, value -> dict) { ... }
    // enum(take elem -> dict) { ... }

    TOKEN_ENUM,

    /** Literals. */

    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NAN,
    TOKEN_INFINITY,
    TOKEN_NULL,

    /** For string interpolation. */

    TOKEN_INTERPOLATION,

    /** For unexpected types. */

    TOKEN_ERROR,
    TOKEN_EOF
} TokenType;

// A single type of token consist a starting point, which points dierectly 
// into the source, the length of the token in the source, line number and 
// assosiated literal value.

typedef struct struct_Token {
    TokenType type;

    // The beginning of the token, pointing directly into the source.

    const char* start;

    // Length of the token in the source.

    int length;

    // The line number form where the token is hailed.

    int line;

    // Parsed value if the token is a literal.

    void* value;
} Token;

typedef struct struct_Scanner {
    // The main source code which is being lexed.

    const char* source;

    // Beginning of the currently lexed token in [source].

    const char* start;

    // The current character being lexed in the [source].

    const char* current;

    // 1-based line number where current scanning is going on.

    int line;
} Scanner;

// Initialize the scanner.

void promit_Scanner_init(Scanner*, const char* const);

// Scan and emit the next token whenever the compiler needs it.

Token promit_Scanner_next_token(Scanner*);

#endif    // __PROMIT_SCANNER_H__