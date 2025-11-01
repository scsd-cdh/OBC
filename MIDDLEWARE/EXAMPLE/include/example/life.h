/*
 * This file contains the public API for this middleware, it should be as small as possible. Defines and macros are
 * especially nasty and should be used sparingly in the public headers as they will spread easily to weird places.
 *
 * Any function defined here is fair game for anyone wanting to call it.
 *
 * Be very careful of what you #include here as its going to be included by anything #include-ing this file.
 */

#pragma once
#include <stdbool.h>

bool example_get_meaning(int *result);
