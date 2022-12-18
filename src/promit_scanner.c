#include <promit_scanner.h>

// void promit_Scanner_init(Scanner*, const char* const);
// 
// Initialize the scanner.

void promit_Scanner_init(Scanner* scanner, const char* source) {
    scanner -> source  = source;
    scanner -> start   = source;
    scanner -> current = source;
    scanner -> line    = 1;
}

// void promit_Scanner_next_token(Scanner*);
// 
// Scan and emit the next token whenever the compiler needs it.

Token promit_Scanner_next_token(Scanner* scanner) {
    
}