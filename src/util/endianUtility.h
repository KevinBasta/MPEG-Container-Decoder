
#ifndef ENDIAN_UTIL_H
#define ENDIAN_UTIL_H

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

// the uses of these need to be converted to the interface below:
u32 bigEndianCharToLittleEndianUnsignedInt(u8 *bigEndianCharArray);
i32 bigEndianCharToLittleEndianInt(u8 *bigEndianCharArray);
////////////////////////////////////////////////////////////////


i32 bigEndianCharToLittleEndianGeneralized(u8 *bigEndianCharArray, u32 numberOfBytes);

u64 simpleBigEndianToLittleEndianBits(u8 *bigEndianCharArray, u32 startingBit, u32 endingBit, u32 numberOfBits);

/**
 * @brief u64val is where the converted int is writte.
 * all types can be read for type conversion.
 */
union endianSwappedInt { 
    u8  u8val;
    u16 u16val;
    u32 u32val;
    u64 u64val;

    i8  i8val;
    i16 i16val;
    i32 i32val;
    i64 i64val;
};

union endianSwappedInt bigEndianIntegerStoredInCharArrayToLittleEndianGeneralizedInteger(u8 *bigEndianCharArray, u32 numberOfBytes);
union endianSwappedInt convertToLittleEndian(u8 *arr, u32 numberOfBytes);
union endianSwappedInt convertToPlatformEndian(u8 *arr, u32 numberOfBytes, bool bigEndianData);

// u8-u64 stack ints
u8 bigEndianU8ArrToLittleEndianU8(u8 *bigEndianU8Arr);
u16 bigEndianU8ArrToLittleEndianU16(u8 *bigEndianU8Arr);
u32 bigEndianU8ArrToLittleEndianU32(u8 *bigEndianU8Arr);
u64 bigEndianU8ArrToLittleEndianU64(u8 *bigEndianU8Arr);

// i8-i64 stack ints
i8 bigEndianU8ArrToLittleEndianI8(u8 *bigEndianU8Arr);
i16 bigEndianU8ArrToLittleEndianI16(u8 *bigEndianU8Arr);
i32 bigEndianU8ArrToLittleEndianI32(u8 *bigEndianU8Arr);
i64 bigEndianU8ArrToLittleEndianI64(u8 *bigEndianU8Arr);

// u8-u64 stack ints interface
u8 littleEndianU8(u8 val);
u16 littleEndianU16(u16 val);
u32 littleEndianU32(u32 val);
u64 littleEndianU64(u64 val);

// i8-i64 stack ints interface
i8 littleEndianI8(i8 val);
i16 littleEndianI16(i16 val);
i32 littleEndianI32(i32 val);
i64 littleEndianI64(i64 val);

#endif // ENDIAN_UTIL_H