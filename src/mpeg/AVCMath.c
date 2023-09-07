
#include <math.h>

#include <stdint.h>
#include "main.h"

#include "AVCMath.h"
#include "bitUtility.h"
#include "endianUtility.h"

/* i32 inverseRasterScan(i32 a, i32 b, i32 c, i32 d, i32 e) { 
    if (e == 0) { 
        return ((a % (d / b)) * b);
    } else if (e == 1) { 
        return ((a / (d / b)) * c);
    }

    return 0;
} */

u32 u(u8 *data, u32 *bitsRead, u32 *bytesRead, u32 numberOfBits) { 
    return getUnsignedNBits(data, bitsRead, bytesRead, numberOfBits);
}

u32 getUnsignedNBits(u8 *data, u32 *bitsRead, u32 *bytesRead, u32 numberOfBits) { 
    u32 preReferenceBitsOffset      = *bitsRead;
    u8 *bigEndianDataReference      = referenceNBits(numberOfBits, data, bitsRead, bytesRead);
    u32 postReferenceBitsOffset     = *bitsRead;
    /* printf("preReferenceBitsOffset %d\n", preReferenceBitsOffset);
    printf("postReferenceBitsOffset %d\n", postReferenceBitsOffset);
    printf("preReferenceBitsOffset+numberOfBits %d\n\n", preReferenceBitsOffset+numberOfBits); */

    u32 readBits                    = simpleBigEndianToLittleEndianBits(bigEndianDataReference, preReferenceBitsOffset, postReferenceBitsOffset-1, numberOfBits);
    return readBits;
}



u32 ue(u8 *data, u32 *bitsRead, u32 *bytesRead, u32 dataLength) { 
    return getCodeNum(data, bitsRead, bytesRead, dataLength);
}

i32 se(u8 *data, u32 *bitsRead, u32 *bytesRead, u32 dataLength) { 
    u32 codeNum = getCodeNum(data, bitsRead, bytesRead, dataLength);
    return (i32) (pow(-1, (codeNum + 1)) * ceil(codeNum / 2));
}

/**
 * @brief ue(v)
 * @param data 
 * @param bitsRead 
 * @param bytesRead 
 * @param dataLength 
 * @return 
 */
u32 getCodeNum(u8 *data, u32 *bitsRead, u32 *bytesRead, u32 dataLength) { 
    u32 leadingZeroBits             = countBitsToFirstNonZero(data, bitsRead, bytesRead, dataLength);
    //printf("bits bytes 1: %d %d\n", *bitsRead, *bytesRead);

    u32 leadingBitsOffsetPlusTwo    = *bitsRead;
    u8 *bigEndianDataReference      = referenceNBits(leadingZeroBits, data, bitsRead, bytesRead);
    u32 postReferenceBitsOffset     = *bitsRead;
    //printf("bits bytes 2: %d %d\n", *bitsRead, *bytesRead);

    u32 readBits                    = simpleBigEndianToLittleEndianBits(bigEndianDataReference, leadingBitsOffsetPlusTwo, postReferenceBitsOffset-1, leadingZeroBits);
    /* printf("leadingBitsOffsetPlusTwo %d\n", leadingBitsOffsetPlusTwo);
    printf("postReferenceBitsOffset %d\n", postReferenceBitsOffset);
    printf("leadingBitsOffsetPlusTwo+leadingZeroBits %d\n", leadingBitsOffsetPlusTwo+leadingZeroBits); */

    /* printf("readBits: %d\n", readBits);
    printf("data: %x %x %x %x\n", bigEndianDataReference[(u32) floor(leadingBitsOffsetPlusTwo / 8.0)],
                                      bigEndianDataReference[(u32) floor(leadingBitsOffsetPlusTwo / 8.0) + 1],
                                      bigEndianDataReference[(u32) floor(leadingBitsOffsetPlusTwo / 8.0) + 2],
                                      bigEndianDataReference[(u32) floor(leadingBitsOffsetPlusTwo / 8.0) + 3]);
    printf("starting bit: %d\n", (leadingBitsOffsetPlusTwo % 8));
    printf("leadingZeroBits: %d\n", leadingZeroBits); */



    //printf("bits bytes 3: %d %d\n", *bitsRead, *bytesRead);
    
    //printBits(bigEndianDataReference, (u32) floor(leadingZeroBits / 8));
    //printf("leading Zeros: %d\n", leadingZeroBits);
    //printf("bits little endian: %d\n", readBits);

    return (u32) (pow(2, leadingZeroBits) - 1 + readBits);
}
