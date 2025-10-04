
#ifndef _UTILS_
#define _UTILS_


#include <stdint.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))


/**
 * @brief Copies a specified number of bytes from a source array to a destination array.
 *
 * @param source Pointer to the source array
 * @param dest Pointer to the destination array
 * @param count Number of bytes to copy
 */
void CopyArray(uint8_t *source, uint8_t *dest, uint8_t count);

#endif // _UTILS_
