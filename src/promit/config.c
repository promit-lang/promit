/** Implements the API functions in 'promit/promit.h'. */

#include <promit/promit.h>

// C standard includes.

#include <stdlib.h>

// Promit's default reallocator function.

static void* default_reallocator(void* memory, size_t size) {
    // If [size] is 0, then this function is called to free the [memory] 
    // pointer.

    IF(size == 0u) {
        free(memory);

        return NULL;
    }

    void* result = realloc(memory, size);

    if(result == NULL) {
        // TODO: Add code.
    }

    return result;
}

// void promit_PromitConfiguration_init(PromitConfiguration*);

void promit_PromitConfiguration_init(PromitConfiguration* config) {
    // Initializes the configuration struct with Promit's preferred 
    // defaults.

    config -> error       = NULL;                   // No error function.
    config -> reallocator = default_reallocator;
}