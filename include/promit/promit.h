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

#include <salamander/salamander.h>
#include <salamander/compiler.h>

/**
 * Changing the SalamanderVM symbols to Promit symbols.
 * 
 * In official Promit implementation, a promit program is compoiled to 
 * series of bytecodes targetting SalamanderVM runtime. SalamanderVM is very 
 * lightweight VM, which does not have a runtime library and requires you to 
 * implement one yourself. This project holds a compiler which compiles promit 
 * program and a runtime core standard library aka Promit Standard Library 
 * (PSL). For portability and integration reasons, the VM is separate from the 
 * compiler. So, the client application embedding this language has to use 
 * both 'promit' and 'salamander' related symbols, e.g. to initialize the VM 
 * the client has to call something like 'promit_Lib_init' and also has to 
 * create a new VM by calling 'salamander_SalamanderVM_new'.
 * 
 * Basically. in order to embedd Promit, you have to keep track to two 
 * seperate symbol class, which is really cumbersome not to mention mass 
 * confusing.
 * 
 * For that reason, this project transforms (more like abstracts) the 
 * 'salamander' symbols with the 'promit' symbols as following: 
 * 
 *     SalamanderVM -> PromitVM
 *     salamander_SalamanderVM_new -> promit_PromitVM_new
 * 
 * etc. to avoid mass confusion.
 * 
 * Note: SalamanderVM is part of Project Promit.
 */

#define PROMIT_API SALAMANDER_API

typedef SalamanderConfiguration PromitConfiguration;
typedef SalamanderVM PromitVM;

PromitVM* (*promit_PromitVM_new)(PromitConfiguration*) = 
    salamander_SalamanderVM_new;

void (*promit_PromitVM_execute)(PromitVM*, ExecutionType, void*) = 
    salamander_SalamanderVM_execute;

void (*promit_PromitVM_free)(PromitVM*) = salamander_SalamanderVM_free;

PROMIT_API ObjFn* promit_Compiler_compiler(PromitVM*, const char*, bool);

#endif    // __PROMIT_H__