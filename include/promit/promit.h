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
    const char* module;
    const char* message;
    const char* line;
    int line_len;
    int line_num;
    int column;    // Column number.
    int length;    // Token length.
} ErrorData;

// Function to call if any compilation error occurs. This function will decide 
// what to do with the error data.

typedef void (*PromitErrorFn)(ErrorData);

CompilerKit* promit_Compiler_compile(SalamanderVM*, const char*, bool, PromitErrorFn);

#endif    // __PROMIT_H__