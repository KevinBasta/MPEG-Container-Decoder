
#ifndef TYPES_UTIL_H
#define TYPES_UTIL_H

#include <stdint.h>
#include "main.h"

u32 binarySearch(u32 target, u32 *table, u32 totalEntries, i32 (*comp) (const void *, const void *));
u32 linearSearch(u32 target, u32 *array, u32 totalEntries, i32 (*comp) (const void *, const void *));
i32 compi32(const void *a, const void *b);
i32 compu32(const void *a, const void *b);

void bubbleSort(u32 *arr1, u32* arr2, int n);
void quickSort(u32 *arr1, u32 *arr2, u32 low, u32 high);

#endif // TYPES_UTIL_H