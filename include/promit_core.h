#ifndef __PROMIT_CORE_H__
#define __PROMIT_CORE_H__

// __builtin_expect(n, e) is an extension to both GCC and clang compiler used
// for branch predictions which optimizes the condition pipeline, thus makes
// the condition related statements run faster.

#if defined __GNUC__ || defined __clang__

#define is_true(x) __builtin_expect(!!(x), 1)
#define is_false(x) __builtin_expect(!!(x), 0)

#else

#define is_true(x) x
#define is_false(x) !x

#endif    // __GNUC__ and __clang__

#ifdef PROMIT_DEBUG

#include <stdio.h>

// Indicates the program execution shouldn't reach the portion of code. If it
// does, dump error in DEBUG mode.
// 
// In release mode, use compiler specific built-in functions to indicate 
// uncreachable codes. It tells the compilers a portion of code should never
// be reached. This gets rid of the 'expected a return' warnings.

#define UNREACHABLE()                                                        \
    do {                                                                     \
        fprintf(stderr,                                                      \
            "[%s:%d] This portion of code should not be reached in %s()!",   \
            __FILE__, __LINE__, __func__);                                   \
        abort();                                                             \
    } while(false)

#else 

// '__assume' in MSVC compiler indicates unreachable code to optimizer where
// '__builtin_unreachable' is used for GCC compilers of version 4.5 or higher.

#if defined _MSC_VER

#define UNREACHABLE() __assume(0)

#elif (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))

#define UNREACHABLE() __builtin_unreachable()

#else 

#define UNREACHABLE() do {} while(false)

#endif    // _MSC_VER and __GNUC__

#endif    // PROMIT_DEBUG

#endif    // __PROMIT_CORE_H__