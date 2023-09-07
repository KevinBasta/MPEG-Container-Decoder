
#ifndef STORAGE_TYPES_H
#define STORAGE_TYPES_H

#include <stdint.h>
#include "main.h"

#include "linkedlist.h"
#include "typesMPEG-4.h"
#include "typesAVC.h"

// Main Storage Structs //

typedef struct sampleInfo {
    u32 realTime; // may remove 
    u32 mediaTime;
    u32 sampleNumber;
    u32 chunkNumber;
    u32 chunkOffset;
    u32 sampleSize;
    u32 sampleIndexInChunk;
    u32 sampleOffsetInChunk;
    u32 sampleOffsetInMdat;
    u8 *mdatPointer;
} sampleInfo;


/**
 * @brief 
 * @param *timescale            -   the number of time units that pass per second in 
 * this movies time coordinate system.
 * @param *fullDuration         -   the duration of the longest trak in the movie in
 * time scale units.
 * @param *videoTrackDuration   -   the duration of the video track in the movie in
 * movie time scale units. derived from the track's edts. equal to the sum of the 
 * durations of all the track's edits. if there is not elst then it's the sum of 
 * all sample durations converted into movie timescale.
 *
 */
typedef struct MPEG_Data {
    // 
    linkedList *topBoxesLL;

    // from mvhdParseBox
    u32 mvhdTimeScale; 
    u32 mvhdDuration;
    
    // from tkhdParseBox
    u32 tkhdTrackDuration;
    
    // from edtsParseBox->elstParseBox
    elstTable *elstTable;
    
    // from mdhdParseBox
    u32 mdhdTimeScale;
    u32 mdhdDuration;

    // from dinfParseBox->drefParseBox
    dataReferenceTableEntry **dataReferenceTable;

    // from sttsParseBox
    timeToSampleTableCompressed *timeToSampleTableCompressed;
    timeToSampleTable *timeToSampleTable;

    // from stscParseBox
    sampleToChunkTable *sampleToChunkTable;

    // from stcoParseBox
    chunkOffsetTable *chunkOffsetTable;

    // from stszParseBox
    sampleSizeTable *sampleSizeTable;
    u32 numberOfSamples; 

    // from stssParseBox
    syncSampleTable *syncSampleTable;

    // from cttsParseBox
    compositionOffsetTableCompressed *compositionOffsetTableCompressed;
    compositionOffsetTable *compositionOffsetTable;

    // mdat Data
    box *mdatBox;
    u32 mdatDataOffsetInFile;

    // 
    displayTimeToSampleTable *displayTimeToSampleTable;

    // avcc length of nal unit
    AVC_Data *avcData;
    

} MPEG_Data;

#endif // STORAGE_TYPES_H
