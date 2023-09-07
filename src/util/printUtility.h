
#ifndef PRINT_UTIL_H
#define PRINT_UTIL_H

#include <stdint.h>
#include "main.h"

void printNBytes(u8 *string, u32 bytesToPrint, u8 prefixString[], u8 postfixString[]);
void printHexNBytes(u8 *string, u32 bytesToPrint);
void printCharArrayBits(u8 *bitPattern);
void printBits(void const * const ptr, size_t const size);
void printIntBits(void const * const ptr, size_t const size);
char* intToString(size_t i);

#endif // PRINT_UTIL_H