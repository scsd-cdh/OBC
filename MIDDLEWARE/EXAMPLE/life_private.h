/*
 * If your file has both private and public members, you should create a <file>_private.h outside the include directory.
 *
 * Effort should still be taken care to make sure to only #include thing you actually need and to minimize defines and
 * macros in this file. However, as only files in this module/middleware are able to include this file, this rule is
 * more relaxed for private headers.
 */

#pragma once

void example_reset_counter();
