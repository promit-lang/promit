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

ObjFn* promit_Compiler_compile(SalamanderVM*, const char*, bool);

#endif    // __PROMIT_H__