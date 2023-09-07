
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <stdint.h>
#include "main.h"

#include "bitUtility.h"

/**
 * @brief Compares n bytes of u8 types. Intended for comparing the 
 * types of MPEG boxes.
 * @param *firstItem    -   n byte u8 array pointer
 * @param secondItem    -   m byte u8 array (includes '\0')
 * @param numberOfBytes -   the number of bytes to compare
 * @return TRUE or FALSE
 */
i32 compareNBytes(u8 *firstItem, u8 secondItem[], u32 numberOfBytes) {     
    i32 isEqual = TRUE;
    for (u32 i = 0; i < numberOfBytes; i++) {
        if (firstItem[i] != secondItem[i]) {
            isEqual = FALSE;
            break;
        }
    }

    return isEqual;
}


/**
 * @brief creates a new array containing n bytes of data from the originalData array 
 * @param numberOfBytes     -   amount of bytes to allocate for returned array
 * @param *originalData     -   pointer to an array
 * @param *byteOffset       -   the current array index of originalData
 * @return a new char array of size numberOfBytes that contains data from
 * originalData
 *
 * @note redundand memory usage if parent array persists.
 * @note it might be benifitial to add a terminating char.
 */
u8 *copyNBytes(u32 numberOfBytes, u8 *originalData, u32 *byteOffset) {
    u8 *infoCopy = (u8*) malloc(numberOfBytes);
    for (u32 i = 0; i < numberOfBytes; i++) {
        infoCopy[i] = originalData[*byteOffset];
        *byteOffset += 1;
    }

    return infoCopy;
}


/**
 * @brief returns a char pointer from an array and increments byteOffset by the 
 * size of the data that the char pointer refers to
 * @param numberOfBytes     -   amount of bytes to add to byteOffset
 * @param *originalData     -   pointer to an array
 * @param *byteOffset       -   the current array index of originalData
 * @return a pointer to the originalData at offset byteOffset
 *
 * @note if parent array is freed data will be lost.
 * @note will need to manually determine where each reference array ends.
 */
u8 *referenceNBytes(u32 numberOfBytes, u8 *originalData, u32 *byteOffset) {
    u8 *infoReference = &(originalData[*byteOffset]);
    *byteOffset += numberOfBytes;

    // checking if referenced properly
    /* for (u32 i = 0; i < numberOfBytes; i++) { 
        assert(&(infoReference[i]) == &(originalData[(*byteOffset) - numberOfBytes + i]));
        assert(infoReference[i] == originalData[(*byteOffset) - numberOfBytes + i]);
    } */

    return infoReference;
}




// The following functions are for AVC and NAL decoding

/**
 * @brief same as referenceNBytes but does not increment byteOffset. 
 * for checking emulation prevention three byte at the end of a NAL unit.
 * this function is for abstracting the line &(originalData[byteOffset]).
 * @param numberOfBytes     - number of bytes to check at current offset
 * @param originalData      - array to return pointer to
 * @param byteOffset        - offset in array
 * @return pointer to array at current byte offset
 */
u8 *checkNextNBytes(u32 numberOfBytes, u8 *originalData, u32 byteOffset) {
    u8 *infoReference = &(originalData[byteOffset]);

    return infoReference;
}


/**
 * @brief gives a pointer to the byte that contains the starting bit
 * @param numberOfBits  - for incrementing offset
 * @param originalData  - for returning pointer
 * @param bitOffset     - for calculating current byte offset
 * @return array byte pointer containing starting bit
 */
u8 *referenceNBits(u32 numberOfBits, u8 *originalData, u32 *bitOffset, u32 *byteOffset) {
    u8 *infoReference = &(originalData[*byteOffset]);
    *bitOffset += numberOfBits;
    *byteOffset = (u32) floor(*bitOffset / 8.0);
    
    // checking if referenced properly
    /* for (u32 i = 0; i < numberOfBytes; i++) { 
        assert(&(infoReference[i]) == &(originalData[(*byteOffset) - numberOfBytes + i]));
        assert(infoReference[i] == originalData[(*byteOffset) - numberOfBytes + i]);
    } */

    return infoReference;
}

/**
 * @brief Count zero bits + first non-zero bit from bitOffset
 * @note For AVC math functions
 * @param originalData          - array pointer
 * @param startingBitInPointer  - 0-7 
 * @param byteBoundary          - last array byte
 * @return number of 0 bits
 */
u32 countBitsToFirstNonZero(u8 *originalData, u32 *bitOffset, u32 *byteOffset, u32 byteBoundaryPlusOne) { 
    // update byteOffset based on bitOffset
    *byteOffset = floor(*bitOffset / 8.0);
    
    u8 bitStart;

    u32 startingBitInPointer = (*bitOffset % 8);
    if (startingBitInPointer < 0 || startingBitInPointer > 7) { 
        bitStart = 0;
    } else { 
        bitStart = startingBitInPointer;
    }

    u32 leadingZeroBits = 0;

    while (TRUE) {
        for (u32 j = bitStart; j < 8; j++) {
            *bitOffset += 1;
            if (getNthBit(j, originalData[*byteOffset]) != 0) { 
                return leadingZeroBits;
            } else {
                leadingZeroBits++;
            }
        }
        
        bitStart = 0;

        if (*byteOffset > byteBoundaryPlusOne - 1) { 
            return 0;
        } else {
            *byteOffset = (u32) floor(*bitOffset / 8.0);
        }
    }

    return 0;
}

/**
 * @brief Compute the min number of bits it takes
 * (with no leading zeros) to represent the integer
 * @note For GIF flexible code sizes
 * @param item integer getting the bitsize of
 * @return number of bits the item occupies
 */
u32 getOccupiedBits(u32 item) {
    // count bits from left side to first non zero bit
    u32 leadingBits = 0;

    for (u32 i = 0; i < sizeof(item) * 8; i++) {
        bool isZero = (item & (1 << ((sizeof(item) * 8) - i - 1))) == 0;

        if (isZero == false) {
            break;
        } else {
            leadingBits++;
        }
    }

    u32 itemSize = (sizeof(item) * 8) - leadingBits;
    //printf("leading bits: %d\n", leadingBits);
    //printf("final size: %d\n", itemSize);
    
    return itemSize;
}

/**
 * @brief get a range of bits shifted to the least segnificant bit. bits numbered 0-7
 * @param startBit the starting bit in the range of 0-7 to include in final u8 bit pattern
 * @param endBit the last bit in the range of 0-7 to include in the final u8 bit pattern.
 * this is the least segnificant bit
 * @param data the data to get the bits from
 * @return u8 with the bit range in the lsb
 */
u8 getNBits(u32 startBit, u32 endBit, u8 data) { 
    if (startBit < 0 || startBit > 7 || endBit > 7 || endBit < 0 || endBit < startBit) { 
        return 0; // considar changing to -1. might end up with other bugs due to i8 in shifting
    }

    u8 leftShift = startBit;
    u8 rightShift = (startBit) + (7 - endBit);

    return (u8) (data << leftShift) >> rightShift;
    // or mask the range startBit to endBit and then shift right
}


u8 getNthBit(u32 bitNumber, u8 data) { 
    if (bitNumber < 0 || bitNumber > 7) { 
        return 0; 
    }

    u8 leftShift = bitNumber;
    u8 rightShift = (bitNumber) + (7 - bitNumber);

    return (u8) (data << leftShift) >> rightShift;
    // or mask the range startBit to endBit and then shift right
}