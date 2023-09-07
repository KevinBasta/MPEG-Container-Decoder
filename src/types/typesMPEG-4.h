
#ifndef MPEG_TYPES_H
#define MPEG_TYPES_H

#include <stdint.h>
#include "main.h"

/**
 * @brief Holds an MPEG-4 Box/Atom. 
 * @param *boxSize  -   4 Bytes for full size of Box/Atom including Header
 * @param *boxType  -   4 Bytes for 4 characters denoting type (no '\0' included)
 * @param *boxData  -   (boxSize - BOX_HEADER_SIZE) length char array for Box/Atom body
 *
 * @note "Header" refers to the boxSize and boxType of a Box/Atom.
 * @note boxData length is calculated with (boxSize - BOX_HEADER_SIZE).
 * @note boxType does NOT include the null terminator '\0'.
 */
typedef struct box { 
    u32 boxSize;
    u8 *boxType;
    u8 *boxData;
} box;

/**
 * @brief Sample boxes sub container
 */
typedef struct sampleDescription { 
    u32 size; 
    u8 *dataFormat;
    u8 *reserved;
    unsigned short dataReferenceIndex;
} sampleDescription;

/**
 * @brief 
 * @param *trackDuration    -   duration of edit/track in movie timescale units
 * @param *mediaTime        -   the starting time within the media of the edit
 * in movie time scale units. this is -1 if the edit is empty. 
 * @param *mediaRate        -   realtive rate which to play the media corresponding
 * to this edit segment. Cannot be 0 or negative.
 */
typedef struct elstTable { 
    u32 totalEntries;
    u32 *trackDurationArr;
    i32 *mediaTimeArr;
    u32 *mediaRateArr;
} elstTable;


// Need to finalize the following types //
/**
 * @brief Dinf box sub container
 */
typedef struct dataReferenceTableEntry { 
    u32 size; 
    u8 *type;
    u8 *version;
    u8 *flags;
    u8 *data;
} dataReferenceTableEntry;

typedef struct sampleToChunkTable { 
    u32 totalEntries;
    u32 *firstChunkArr;
    u32 *samplesPerChunkArr;
    u32 *sampleDescriptionIdArr;
} sampleToChunkTable;

typedef struct chunkOffsetTable { 
    u32 totalEntries; 
    u32 *offsetArr;
} chunkOffsetTable;


typedef struct sampleSizeTable { 
    u32 totalEntries;
    u32 sampleSizeDefault;
    u32 *sizeArr;
} sampleSizeTable;


// Two main types of MPEG types //

// First type: Compressed Data
// Stored as array of type entries
typedef struct timeToSampleTableCompressed { 
    u32 totalEntries;
    u32 *sampleCountArr; 
    u32 *sampleDurationArr;
} timeToSampleTableCompressed;

typedef struct compositionOffsetTableCompressed { 
    u32 totalEntries;
    u32 *sampleCountArr;
    i32 *compositionOffsetArr;
} compositionOffsetTableCompressed;


// Second type: uncompressed Data
// Stored as a type containing arrays
typedef struct syncSampleTable { 
    u32 totalEntries;
    u32 *sampleNumberArr;
} syncSampleTable;

typedef struct timeToSampleTable { 
    u32 totalEntries;
    u32 *sampleDurationArr;
    u32 *sampleDeltaArr;
} timeToSampleTable;

typedef struct compositionOffsetTable { 
    u32 totalEntries;
    i32 *compositionOffsetArr;
} compositionOffsetTable;

typedef struct displayTimeToSampleTable { 
    u32 totalEntries;
    u32 *displayTimeArr;
    u32 *sampleNumberArr;
} displayTimeToSampleTable;

#endif // MPEG_TYPES_H
