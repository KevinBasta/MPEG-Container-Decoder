
#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

// Preprocessor constants
#define BOX_HEADER_SIZE 8
#define BOX_HEADER_HALF_SIZE 4 // unused
#define BOX_SIZE_SIZE 4
#define BOX_LONG_SIZE_SIZE 8 // verify
#define BOX_TYPE_SIZE 4
#define BOX_VERSION_SIZE 1
#define BOX_FLAG_SIZE 3
#define TRUE 1
#define FALSE 0
#define DEBUG_PRINT_ENABLE TRUE
#define DEBUG_PRINT(...)  print(__VA_ARGS__)
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#define PRINT_INT(X) printf("%d\n", X) 

// renaming stdint types
typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// math functions
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

// error handling
typedef enum STATUS_CODE {
    OPERATION_SUCCESS = 1, 
    OPERATION_FAILED  = 0
} STATUS_CODE;

#define CHECKSTATUS(status)                     if (status == OPERATION_FAILED) { return OPERATION_FAILED; }
#define CHECK_FWRITE_STATUS(returned, expected) if (returned != expected)       { return OPERATION_FAILED; }
#define CHECK_NULL_RETURN(status)               if (status == NULL) { return OPERATION_FAILED; }

#endif //MAIN_H