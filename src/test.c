#include <assert.h>   
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
//#include <CUnit/CUnit.h>

#ifndef COMMON_TYPES
    #define COMMON_TYPES
    #include "main.h"
    #include "typesMPEG-4.h"
    #include "typesAVC.h"
    #include "linkedlist.h"
    #include "typesStorage.h"
    #include "typesUtility.h"
#endif //COMMON_TYPES

#ifndef COMMON_UTIL
    #define COMMON_UTIL
    #include "bitUtility.h"
    #include "endianUtility.h"
    #include "printUtility.h"
#endif //COMMON_UTIL


int main() { 

    assert(getNBits(0, 2, 0xFF) == 7);
    assert(getNBits(2, 4, 0x5C) == 3);
    assert(getNBits(0, 0, 0x8C) == 1);
    assert(getNBits(0, 7, 0x8C) == 0x8C);
    assert(getNBits(8, 4, 0x5C) == 0);
    assert(getNBits(-1, 4, 0x5C) == 0);
    assert(getNBits(3, 0, 0x5C) == 0);

    assert(getNthBit(0, 0x8C) == 1);
    assert(getNthBit(2, 0xFF) == 1);
    assert(getNthBit(4, 0x5C) == 1);
    assert(getNthBit(6, 0x98) == 0);
    assert(getNthBit(7, 0x9E) == 0);
    assert(getNthBit(7, 0x9F) == 1);
    assert(getNthBit(8, 0x5C) == 0);
    assert(getNthBit(-1, 0x5C) == 0);

    u8 *testArr = (u8*) malloc(sizeof(u8) * 10);
    testArr[0] = 0x00; testArr[1] = 0x01; // 0000 0000 0000 0001
    u32 byteOffset = 0;
    i32 bitOffset = 4;
    assert(countBitsToFirstNonZero(testArr, &bitOffset, &byteOffset, 9) == 11); byteOffset = 0; bitOffset = 7;
    assert(countBitsToFirstNonZero(testArr, &bitOffset, &byteOffset, 9) == 8); byteOffset = 0; bitOffset = 0;
    assert(countBitsToFirstNonZero(testArr, &bitOffset, &byteOffset, 9) == 15); byteOffset = 0; bitOffset = 2;
    assert(countBitsToFirstNonZero(testArr, &bitOffset, &byteOffset, 9) == 13); byteOffset = 0; bitOffset = 8;
    assert(countBitsToFirstNonZero(testArr, &bitOffset, &byteOffset, 9) == 7); byteOffset = 0; bitOffset = 0;

    for (int i = 0; i < 10; i++) {
        testArr[i] = 0;
    }
    testArr[0] = 0x08; testArr[1] = 0x08; // 0000 (1000 0000 1)000
    assert(simpleBigEndianToLittleEndianBits(testArr, 4, 12, 9) == 257);
    testArr[0] = 0x01; testArr[1] = 0x20; // 0000 000(1 001)0 0000
    assert(simpleBigEndianToLittleEndianBits(testArr, 7, 10, 4) == 9);
    testArr[0] = 0x01; testArr[1] = 0x20; // 0000 000(1 0010) 0000
    assert(simpleBigEndianToLittleEndianBits(testArr, 7, 11, 5) == 18);
    testArr[0] = 0x11; testArr[1] = 0x80; // 000(1 0001 1)000 0000
    assert(simpleBigEndianToLittleEndianBits(testArr, 3, 8, 6) == 35);
    testArr[0] = 0x6F; testArr[1] = 0x41; // (0110 1111 0100 0001)
    assert(simpleBigEndianToLittleEndianBits(testArr, 0, 15, 16) == 0x6F41);
    
    testArr[0] = 0x6F; testArr[1] = 0x41; // (011 0111 1010 0000)1 
    assert(simpleBigEndianToLittleEndianBits(testArr, 0, 14, 15) == 0x37A0);
    
    testArr[0] = 0x6F; testArr[1] = 0x41; // 0(110 1111 0100 0001) 
    assert(simpleBigEndianToLittleEndianBits(testArr, 1, 15, 15) == 0x6F41);
    
    testArr[0] = 0x6F; testArr[1] = 0x41; // 0(1 1011 1101 0000)01
    assert(simpleBigEndianToLittleEndianBits(testArr, 1, 13, 13) == 0x1BD0); 
    
    testArr[0] = 0x6F; testArr[1] = 0x41; // 0(1101 1110 1000)001
    assert(simpleBigEndianToLittleEndianBits(testArr, 1, 12, 12) == 0xDE8); 

    testArr[0] = 0x6F; testArr[1] = 0x41; // 0110 1111 0100 0001 
    assert(simpleBigEndianToLittleEndianBits(testArr, 0, 7, 8) == 0x6F);
    testArr[0] = 0x6F; testArr[1] = 0x41; testArr[2] = 0x6F; testArr[3] = 0x41; // 0110 1111 0100 0001 
    assert(simpleBigEndianToLittleEndianBits(testArr, 0, 31, 32) == 0x6F416F41);
    testArr[0] = 0x6F; testArr[1] = 0x41; testArr[2] = 0x6F; testArr[3] = 0x41; testArr[4] = 0x6F; testArr[5] = 0x41;
    //printf("%lx\n",simpleBigEndianToLittleEndianBits(testArr, 0, 47, 48));
    


    //u8 test = getNBits(1, 1, 0x8C);
    //printBits(&test, 1);

    free(testArr);
    printf("All Assertions Passed\n");
}

