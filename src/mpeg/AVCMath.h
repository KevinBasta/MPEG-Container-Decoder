
#ifndef AVC_MATH_H
#define AVC_MATH_H

#include <stdint.h>
#include "main.h"

//i32 inverseRasterScan(i32 a, i32 b, i32 c, i32 d, i32 e);
u32 u(u8 *data, u32 *bitsRead, u32 *bytesRead, u32 numberOfBits);
u32 getUnsignedNBits(u8 *data, u32 *bitsRead, u32 *bytesRead, u32 numberOfBits);

u32 ue(u8 *data, u32 *bitsRead, u32 *bytesRead, u32 dataLength);
i32 se(u8 *data, u32 *bitsRead, u32 *bytesRead, u32 dataLength);
u32 getCodeNum(u8 *data, u32 *bitsRead, u32 *bytesRead, u32 dataLength);

#endif // AVC_MATH_H
