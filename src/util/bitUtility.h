
#ifndef BIT_UTIL_H
#define BIT_UTIL_H

#include <stdint.h>
#include "main.h"

i32 compareNBytes(u8 *firstItem, u8 secondItem[], u32 numberOfBytes);
u8 *copyNBytes(u32 numberOfBytes, u8 *originalData, u32 *byteOffset);
u8 *referenceNBytes(u32 numberOfBytes, u8 *originalData, u32 *byteOffset);

u8 *checkNextNBytes(u32 numberOfBytes, u8 *originalData, u32 byteOffset);
u8 *referenceNBits(u32 numberOfBits, u8 *originalData, u32 *bitOffset, u32 *byteOffset);
u32 countBitsToFirstNonZero(u8 *originalData, u32 *bitOffset, u32 *byteOffset, u32 byteBoundary);
u32 getOccupiedBits(u32 item);
u8 getNBits(u32 startBit, u32 endBit, u8 data);
u8 getNthBit(u32 bitNumber, u8 data);

#endif // BIT_UTIL_H