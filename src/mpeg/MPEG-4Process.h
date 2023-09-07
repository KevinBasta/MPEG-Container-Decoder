
#ifndef MPEG_PROCESS_H
#define MPEG_PROCESS_H

#include <stdint.h>
#include "main.h"
#include "typesMPEG-4.h"
#include "typesStorage.h"

u32 realTimeToMediaTime(u32 time, u32 newTimeScale);
u32 mediaTimeToDecodeSampleNumber(u32 mediaTime, timeToSampleTableCompressed *timeToSampleTable);
u32 mediaTimeToDisplaySampleNumber(u32 mediaTime, displayTimeToSampleTable *displayTimeToSampleTable);
u32 sampleNumberToSampleSize(u32 sampleNumber, sampleSizeTable *sampleSizeTable);
u32 sampleNumberToChunkNumber(sampleInfo *sample, sampleToChunkTable *sampleToChunkTable, sampleSizeTable *sampleSizeTable, u32 numberOfSamples);
u32 getSampleOffsetInChunk(sampleInfo *sample, u32 sampleSizeDefault, sampleSizeTable *sampleSizeTable);
u32 offsetDataToSampleMdatOffset(u32 chunkOffset, u32 sampleOffsetInChunk, u32 mdatOffsetInFile);

void sampleRealTimeToMediaTime(sampleInfo *sample, MPEG_Data *videoData);
void sampleMediaTimeToDecodeSampleNumber(sampleInfo *sample, MPEG_Data *videoData);
void sampleMediaTimeToDisplaySampleNumber(sampleInfo *sample, MPEG_Data *videoData);
void sampleSampleNumberToChunkNumber(sampleInfo *sample, MPEG_Data *videoData);
void sampleChunkNumberToChunkOffset(sampleInfo *sample, MPEG_Data *videoData);
void sampleSampleNumberToSampleSize(sampleInfo *sample, MPEG_Data *videoData);
void sampleOffsetDataToSampleMdatOffset(sampleInfo *sample, MPEG_Data *videoData);
void createDisplayTimeToSampleTable(MPEG_Data *videoData);

void getVideoDataRangeByMediaTime(u32 startTime, u32 endTime, MPEG_Data *videoData);
void getVideoDataRangeBySampleNumber(u32 startTime, u32 endTime, MPEG_Data *videoData); 
sampleInfo *sampleSearchByRealTime(u32 time, MPEG_Data *videoData);
sampleInfo *sampleSearchByMediaTime(u32 mediaTime, u32 previousSampleNumber, MPEG_Data *videoData);
sampleInfo *sampleSearchBySampleNumber(u32 sampleNumber, MPEG_Data *videoData);
sampleInfo *keyFrameSearch(u32 time, MPEG_Data *videoData);

#endif // MPEG_PROCESS_H
