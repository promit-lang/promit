/**
 * promit.h
 * 
 * See the 'LICENSE' file for this file's license.
 *
 * This header file provides all the needed symbol to compile a Promit program 
 * and interpret using SalamanderVM's runtime. This project specifically 
 * focuses on building a compiler generating bytecodes targetting SalamanderVM 
 * and defining the core standard library suitable for Promit Programming 
 * language.
 * 
 * Note: This header file also provides the SVM runtime functions to interpret 
 * the bytecode. Unless you are making a module solely for the VM, you should
 * only include this header. Otherwise, just include the SVM specific header 
 * files.
 *
 * @authors:
 *   1. SD Asif Hossein <s.dah.ingularity@gmail.com> 17th Dec. 2022
 */

#ifndef __PROMIT_H__
#define __PROMIT_H__

// C standard includes.

#include <stdbool.h>

// The SalamanderVM runtime and it's compiler backend.

#include <salamander/compiler.h>

// A struct to fill in the error data.

typedef struct struct_ErrorData {
    // The module name/filename where the error is from.

    const char* module;

    // Error message.

    const char* message;

    // The line where the error is occured.

    const char* line;

    // The line number.

    int line_num;

    // The column number.

    int column;

    // Token length.

    int length;
} ErrorData;

// Function to call if any compilation error occurs. This function will decide 
// what to do with the error data.

typedef void (*PromitErrorFn)(ErrorData*);

// A generic user definable allocator function used for Compiler memory 
// manangement. The function definition should look like this: 
// 
// void* my_reallocator(void* memory, size_t new_size) { --CODE-- }
// 
// The function will be used like following: 
// 
// (1) To allocate new memory of [new_size] bytes and assign it to a new
//     pointer -> [memory] is 'NULL' and [new_size] is a non-zero positive 
//     value.
// (2) To deallocate the existing memory of a pointer -> [memory] is a valid 
//     pointer and [new_size] is zero (0).
// (3) To extend or shrink the existing memory pointer -> [memory] is valid 
//     pointer and [new_size] is a non-zero positive value.

typedef void* (*PromitReallocatorFn)(void*, size_t);

// TODO: Add configuration comments.

typedef struct struct_PromitConfiguration {
    // Error function to call if any compilation error occurs.

    PromitErrorFn error;

    // Default reallocator function to call to allocate, deallocate and 
    // reallocate memory.

    PromitReallocatorFn reallocator;
} PromitConfiguration;

// Initializes the configuration struct with deafult configurations.

void promit_PromitConfiguration_init(PromitConfiguration*);

CompilerKit* promit_Compiler_compile(SalamanderVM*, const char*, bool, PromitConfiguration*);

#endif    // __PROMIT_H__