// Main associated header is always at the top, always in quotes
#include "example/life.h"

// The stdlib and stuff from other modules, always in brackets
#include <stdbool.h>

// Finally, private module includes, in quotes
#include "life_private.h"
#include "supercomputer.h"

// The static keyword is a simple way to restrict a global or a function to this file and this file only.
static int counter = 0;

// This function is considered public because its in the header in the include folder
bool example_get_meaning(int* result) {
    *result = example_compute_meaning();
    counter++;
    return true;
}

// This function is considered private to only this middleware/module because the definition is in a header not in
// the include folder
void example_reset_counter() {
    counter = 0;
}