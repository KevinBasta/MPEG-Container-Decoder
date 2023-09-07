
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <stdint.h>
#include "main.h"

#include "linkedlist.h"
#include "typesMPEG-4.h"
#include "typesStorage.h"
#include "typesUtility.h"

#include "MPEG-4Process.h"

/**
 * @brief converts a real time in a real time coordinate system
 * to media time in media time coordinate system
 * @param realTimeInSeconds     -   with the decimal being milliseconds
 * @param mediaTimeScale        -   used for conversion
 * @return the time in media timescale that corresponds to the real time
 */
u32 realTimeToMediaTime(u32 realTimeInSeconds, u32 mediaTimeScale) { 
    // can put the to seconds conversion here later once interface is decided
    // need to use milliseconds too.  ideally a web interface would allow for 
    // precise time input.

    // testing adding milliseconds
    // u32 convertedTime = ((double) realTimeInSeconds + 0.058) * mediaTimeScale;
    
    // it's hard to get the time for the exact last sample in a movie
    u32 convertedTime = realTimeInSeconds * mediaTimeScale;
    //printf("converted time: %d\n", convertedTime);
    return convertedTime;
}


/**
 * @brief 
 * @param mediaTime             -   time to search for
 * @param timeToSampleTable     -   table to search in
 * @return sampleNumber
 */
u32 mediaTimeToDecodeSampleNumber(u32 mediaTime, timeToSampleTableCompressed *timeToSampleTable) { 
    // need to consider edge cases here
    u32 sampleNumber = 0; 
    u32 sampleTimeAccumulator = 0;

    for (u32 i = 0; i < timeToSampleTable->totalEntries; i++) {
        u32 sampleCountInTableEntry = timeToSampleTable->sampleCountArr[i];
        u32 sampleDurationInTableEntry = timeToSampleTable->sampleDurationArr[i];
        
        for (u32 i = 0; i < sampleCountInTableEntry; i++) { 
            if (sampleTimeAccumulator + sampleDurationInTableEntry < mediaTime){ 
                sampleNumber++;
                sampleTimeAccumulator += sampleDurationInTableEntry;
                //printf("%d %d\n", sampleCountInTableEntry);
            } else { 
                printf("samples total: %d\n", sampleCountInTableEntry);
                return sampleNumber;
            }
        }
    }

    return 0; // error return
}


u32 mediaTimeToDisplaySampleNumber(u32 mediaTime, displayTimeToSampleTable *displayTimeToSampleTable) { 
    //float startTime = (float)clock()/CLOCKS_PER_SEC;
    u32 index = binarySearch(mediaTime, displayTimeToSampleTable->displayTimeArr, displayTimeToSampleTable->totalEntries, compu32);
    //float endTime = (float)clock()/CLOCKS_PER_SEC;
    //float timeElapsed = endTime - startTime;
    //printf("search elapsed: %f\n", timeElapsed);
    return displayTimeToSampleTable->sampleNumberArr[index];
}


/**
 * @brief to allow for default sample size check incase no sample size table present.
 * +1 is needed to convert from 0 indexed array to 1 indexed tables.
 * @param sampleNumber  -   the sample whose size is requested
 * @param videoData     -   for accessing sampleSize and sampleSizeTable
 * @return  the size corresponding to the sample number
 */
u32 sampleNumberToSampleSize(u32 sampleNumber, sampleSizeTable *sampleSizeTable) { 
    if (sampleSizeTable->sampleSizeDefault != 0) { 
        return sampleSizeTable->sampleSizeDefault;
    } 

    return sampleSizeTable->sizeArr[sampleNumber - 1];
}


/**
 * @brief sample needs to be passed in since multiple sampleInfo fields are populated
 * @param sample                -   used to read sampleNumber and store chunkNumber, 
 * sampleIndexInChunkNumber, and sampleOffsetInChunkNumber
 * @param sampleToChunkTable    -   for getting chunkNumber
 * @param numberOfSamples       -   for calculating lastChunk
 * @param sampleSizeTable       -   for calculating sampleOffsetInChunk
 * @param sampleSizeDefault     -   for calculating sampleOffsetInChunk
 * @return the chunkNumber that the sampleNumber resides in
 */
u32 sampleNumberToChunkNumber(sampleInfo *sample, sampleToChunkTable *sampleToChunkTable, sampleSizeTable *sampleSizeTable, u32 numberOfSamples) { 
    // Data read from sample
    u32 sampleNumber = sample->sampleNumber;

    // Data written to sample
    u32 chunkNumber = 0;
    u32 sampleIndexInChunk = 0;
    u32 sampleOffsetInChunk = 0;

    u32 totalSamples = 0;

    for (u32 i = 0; i < sampleToChunkTable->totalEntries; i++) {
        u32 firstChunk = sampleToChunkTable->firstChunkArr[i];
        u32 samplesPerChunk = sampleToChunkTable->samplesPerChunkArr[i];
        
        // "last" refers to the last chunk in this range before next table entry
        u32 lastChunk; 

        // the -1 is imporant here, causes segfault without due to arr access
        if (i + 1 <= (sampleToChunkTable->totalEntries - 1)) { 
            lastChunk = sampleToChunkTable->firstChunkArr[i + 1] - 1;
        } else { 
            lastChunk = firstChunk + (numberOfSamples / samplesPerChunk);
        }

        u32 chunksInChunkRange = lastChunk - firstChunk + 1;
        u32 samplesInChunkRange = chunksInChunkRange * samplesPerChunk;
        // DEBUG printf("%d %d %d %d %d\n", lastChunk, firstChunk, samplesPerChunk, samplesInChunkRange, totalSamples);

        if (sampleNumber <= (samplesInChunkRange + totalSamples)) { 
            for (u32 j = firstChunk; j <= lastChunk; j++) { 
                // DEBUG printf("total samples loop: %d\n", totalSamples);
                chunkNumber++;
                
                if (sampleNumber <= (samplesPerChunk + totalSamples)) { 
                    // NOTE: may be replacable by an equation
                    for (u32 k = 0; k < samplesPerChunk; k++) { 
                        totalSamples += 1;
                        if (sampleNumber <= totalSamples) { 
                            sample->sampleIndexInChunk = sampleIndexInChunk;
                            sample->sampleOffsetInChunk = sampleOffsetInChunk;
                            sample->chunkNumber = chunkNumber;
                            return chunkNumber;
                        } else { 
                            sampleIndexInChunk += 1;
                            sampleOffsetInChunk += sampleNumberToSampleSize(totalSamples, sampleSizeTable);
                        }
                    }
                } else { 
                    totalSamples += samplesPerChunk;
                }
            }
        } else { 
            totalSamples += samplesInChunkRange;
            chunkNumber += chunksInChunkRange;
            // DEBUG printf("%d\n", chunkNumber);
        }
        
        //i++;
    }

    // DEBUG printf("total samples: %d\n", totalSamples);
    sample->sampleIndexInChunk = sampleIndexInChunk;
    sample->sampleOffsetInChunk = sampleOffsetInChunk;
    sample->chunkNumber = chunkNumber;
    return 0; // error return
}


/**
 * @brief mainly to avoid forgetting +1 since the table is 1 indexed but the array is 0 indexed
 * @param chunkNumber           -   chunk to search for
 * @param chunkOffsetTable      -   table to search in
 * @return chunk offset relative to the start of the file. NOT relative to any box.
 */
u32 chunkNumberToChunkOffset(u32 chunkNumber, chunkOffsetTable *chunkOffsetTable) { 
    return chunkOffsetTable->offsetArr[chunkNumber - 1];
}


// sampleSizeAndChunkOffsetToSampleOffsetInChunk
u32 getSampleOffsetInChunk(sampleInfo *sample, u32 sampleSizeDefault, sampleSizeTable *sampleSizeTable) { 
    // for general case when not in array

    u32 offsetAccumulator = 0;
    u32 sampleNumber = sample->sampleNumber;

    for (u32 i = 1; i < sample->sampleIndexInChunk; i++) { 
        offsetAccumulator += sampleNumberToSampleSize(sampleNumber - i, sampleSizeTable);
    }

    return offsetAccumulator;
}


u32 offsetDataToSampleMdatOffset(u32 chunkOffset, u32 sampleOffsetInChunk, u32 mdatDataOffsetInFile) { 
    u32 sampleMdatOffset = chunkOffset + sampleOffsetInChunk - mdatDataOffsetInFile;
    return sampleMdatOffset;
}



// Interface for working with a sampleInfo struct //

/**
 * @brief dependant on sample realTime
 */
void sampleRealTimeToMediaTime(sampleInfo *sample, MPEG_Data *videoData) {
    sample->mediaTime = realTimeToMediaTime(sample->realTime, videoData->mdhdTimeScale);
}

/**
 * @brief dependant on sampleRealTimeToMediaTime
 */
void sampleMediaTimeToDecodeSampleNumber(sampleInfo *sample, MPEG_Data *videoData) { 
    sample->sampleNumber = mediaTimeToDecodeSampleNumber(sample->mediaTime, videoData->timeToSampleTableCompressed);
}

void sampleMediaTimeToDisplaySampleNumber(sampleInfo *sample, MPEG_Data *videoData) { 
    sample->sampleNumber = mediaTimeToDisplaySampleNumber(sample->mediaTime, videoData->displayTimeToSampleTable);
}

/**
 * @brief dependant on sampleMediaTimeToSampleNumber
 */
void sampleSampleNumberToChunkNumber(sampleInfo *sample, MPEG_Data *videoData) { 
    sampleNumberToChunkNumber(sample, videoData->sampleToChunkTable, videoData->sampleSizeTable, videoData->numberOfSamples);
}

/**
 * @brief dependant on sampleSampleNumberToChunkNumber
 */
void sampleChunkNumberToChunkOffset(sampleInfo *sample, MPEG_Data *videoData) { 
    sample->chunkOffset = chunkNumberToChunkOffset(sample->chunkNumber, videoData->chunkOffsetTable);
}

/**
 * @brief dependant on sampleMediaTimeToSampleNumber or just sampleNumber
 */
void sampleSampleNumberToSampleSize(sampleInfo *sample, MPEG_Data *videoData) { 
    sample->sampleSize = sampleNumberToSampleSize(sample->sampleNumber, videoData->sampleSizeTable);
}

/**
 * @brief dependant on sampleSampleNumberToChunkNumber
 */
void sampleOffsetDataToSampleMdatOffset(sampleInfo *sample, MPEG_Data *videoData) { 
    sample->sampleOffsetInMdat = offsetDataToSampleMdatOffset(sample->chunkOffset, sample->sampleOffsetInChunk, videoData->mdatDataOffsetInFile);
}


void createDisplayTimeToSampleTable(MPEG_Data *videoData) { 
    // expand out time to sample
    // expand out composition offset
    u32 numberOfSamples = videoData->numberOfSamples;
    
    if (videoData->compositionOffsetTable == NULL) { 
        displayTimeToSampleTable *table = (displayTimeToSampleTable*) malloc(sizeof(displayTimeToSampleTable));
        u32 *sampleNumberArr = (u32*) calloc(numberOfSamples, sizeof(u32));
        table->totalEntries = numberOfSamples;
        table->displayTimeArr = videoData->timeToSampleTable->sampleDeltaArr;
        table->sampleNumberArr = sampleNumberArr;


        for (u32 i = 0; i < numberOfSamples; i++) {
            sampleNumberArr[i] = i; // should make it i + 1?
        }
        videoData->displayTimeToSampleTable = table;
    
        return;
    }


    displayTimeToSampleTable *table = (displayTimeToSampleTable*) malloc(sizeof(displayTimeToSampleTable));
    u32 *displayTimeArr = (u32*) calloc(numberOfSamples, sizeof(u32));
    u32 *sampleNumberArr = (u32*) calloc(numberOfSamples, sizeof(u32));
    table->totalEntries = numberOfSamples;
    table->displayTimeArr = displayTimeArr;
    table->sampleNumberArr = sampleNumberArr;

    for (u32 i = 0; i < numberOfSamples; i++) { 
        displayTimeArr[i] = videoData->timeToSampleTable->sampleDeltaArr[i] + videoData->compositionOffsetTable->compositionOffsetArr[i];
        sampleNumberArr[i] = i; // should make it i + 1?
    }

    float startTime = (float)clock()/CLOCKS_PER_SEC;
    // quickSort(displayTimeArr, sampleNumberArr, 0, numberOfSamples - 1);
    bubbleSort(displayTimeArr, sampleNumberArr, numberOfSamples);
    float endTime = (float)clock()/CLOCKS_PER_SEC;
    float timeElapsed = endTime - startTime;
    printf("sort elapsed: %f\n", timeElapsed);
    /* for (int i = 0; i < 20; i++) { 
        printf("%d %d \n", displayTimeArr[i], sampleNumberArr[i]);
    } */

    videoData->displayTimeToSampleTable = table;
}



void getVideoDataRangeBySampleNumber(u32 startTime, u32 endTime, MPEG_Data *videoData) { 
    sampleInfo *startSample = sampleSearchByRealTime(startTime, videoData);
    sampleInfo *endSample = sampleSearchByRealTime(endTime, videoData);

    u32 sampleRange = endSample->sampleNumber - startSample->sampleNumber;
    sampleInfo **sampleRangeArray = (sampleInfo**) calloc(sampleRange + 1, sizeof(sampleInfo*));
    sampleRangeArray[sampleRange] = NULL;
    sampleRangeArray[0] = startSample;
    sampleRangeArray[sampleRange - 1] = endSample;

    u32 iterSampleNumber = startSample->sampleNumber;
    for (u32 i = 1; i < (sampleRange - 1); i++) {
        iterSampleNumber += 1;
        sampleRangeArray[i] = sampleSearchBySampleNumber(iterSampleNumber, videoData);
    }
}


void getVideoDataRangeByMediaTime(u32 startTime, u32 endTime, MPEG_Data *videoData) { 
    
    printf("=====================================\n");
    sampleInfo *startSample = sampleSearchByRealTime(startTime, videoData);
    printf("=====================================\n");
    sampleInfo *endSample = sampleSearchByRealTime(endTime, videoData);
    printf("=====================================\n");
    
    //printf("Start Sample\n");
    //parseAVCSample(startSample, videoData);

    linkedList *sampleLL = linkedlistInit();
    linkedlistAppendNode(sampleLL, startSample);

    u32 previousSampleNumber = startSample->sampleNumber;
    int j = 0;
    for (u32 i = startSample->mediaTime + 1; i < endSample->mediaTime - 1; i++) {        
        sampleInfo *sample = sampleSearchByMediaTime(i, previousSampleNumber, videoData);

        if (sample != NULL) { 
            j++;
            previousSampleNumber = sample->sampleNumber;
            linkedlistAppendNode(sampleLL, sample);
            //printf("j: %d\n", j);
            //parseAVCSample(sample, videoData);
        }
    }

    if (((sampleInfo*) sampleLL->last->currentItem)->sampleNumber != endSample->sampleNumber) { 
        linkedlistAppendNode(sampleLL, endSample);
    } else { 
        // does media time of end sample be considered?
        printf("%d %d\n", ((sampleInfo*) sampleLL->last->currentItem)->mediaTime, endSample->mediaTime);
    }

    printf("total: %d\n", sampleLL->size);
}

sampleInfo *sampleSearchByRealTime(u32 time, MPEG_Data *videoData) { 
    sampleInfo *sample = (sampleInfo*) malloc(sizeof(sampleInfo));
    sample->realTime = time;
    sampleRealTimeToMediaTime(sample, videoData);
    //sampleMediaTimeToDecodeSampleNumber(sample, videoData);
    sampleMediaTimeToDisplaySampleNumber(sample, videoData);
    
    printf("sample: %d\n", sample->sampleNumber);

    sampleSampleNumberToChunkNumber(sample, videoData);
    sampleChunkNumberToChunkOffset(sample, videoData);
    sampleSampleNumberToSampleSize(sample, videoData);
    sampleOffsetDataToSampleMdatOffset(sample, videoData);

    printf("media time: %d\n", sample->mediaTime);
    printf("chunk: %d\n", sample->chunkNumber);
    printf("chunk offset: %d\n", sample->chunkOffset);
    printf("sample size: %d\n", sample->sampleSize);
    printf("sample index in chunk: %d\n", sample->sampleIndexInChunk);
    printf("sample offset in chunk: %d\n", sample->sampleOffsetInChunk);
    printf("total samples: %d\n", videoData->numberOfSamples);

    return sample;
}

sampleInfo *sampleSearchByMediaTime(u32 mediaTime, u32 previousSampleNumber, MPEG_Data *videoData) { 
    sampleInfo *sample = (sampleInfo*) malloc(sizeof(sampleInfo));
    sample->mediaTime = mediaTime;
    sampleMediaTimeToDisplaySampleNumber(sample, videoData);
    
    if (sample->sampleNumber == previousSampleNumber) { 
        //printf("same as last sample\n");
        free(sample);
        return NULL;
    }

    sampleSampleNumberToChunkNumber(sample, videoData);
    sampleChunkNumberToChunkOffset(sample, videoData);
    sampleSampleNumberToSampleSize(sample, videoData);
    sampleOffsetDataToSampleMdatOffset(sample, videoData);

    return sample;
}


sampleInfo *sampleSearchBySampleNumber(u32 sampleNumber, MPEG_Data *videoData) { 
    sampleInfo *sample = (sampleInfo*) malloc(sizeof(sampleInfo));
    sample->sampleNumber = sampleNumber;

    sampleSampleNumberToChunkNumber(sample, videoData);
    sampleChunkNumberToChunkOffset(sample, videoData);
    sampleSampleNumberToSampleSize(sample, videoData);
    sampleOffsetDataToSampleMdatOffset(sample, videoData);

    return sample;
}


sampleInfo *keyFrameSearch(u32 time, MPEG_Data *videoData) { 
    sampleInfo *sample = (sampleInfo*) malloc(sizeof(sampleInfo));
    sample->realTime = time;

    sampleRealTimeToMediaTime(sample, videoData);
    sampleMediaTimeToDecodeSampleNumber(sample, videoData);
    u32 keyframe = binarySearch(sample->sampleNumber, videoData->syncSampleTable->sampleNumberArr, videoData->syncSampleTable->totalEntries, compu32);
    printf("keyframe: %d\n", keyframe);

    sampleSampleNumberToChunkNumber(sample, videoData);
    sampleChunkNumberToChunkOffset(sample, videoData);
    sampleSampleNumberToSampleSize(sample, videoData);
    sampleOffsetDataToSampleMdatOffset(sample, videoData);

    return sample;
}
