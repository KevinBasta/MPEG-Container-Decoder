
#ifndef AVC_DECODE_H
#define AVC_DECODE_H

#include <stdint.h>
#include "main.h"
#include "typesStorage.h"
#include "typesAVC.h"

void parseAVCSample(sampleInfo *sampleData, MPEG_Data *videoData);
NALUnitInfo *parseNALUnit(u32 NALUnitDataLength, u8 *NALDataStream);

seqParameterSet *seqParameterSetRbspDecode(u32 NALUnitDataLength, u8 *NALDataStream);
picParameterSet *picParameterSetRbspDecode(u32 NALUnitDataLength, u8 *NALDataStream);
void sliceHeaderDecode(u32 NALUnitDataLength, u8 *NALDataStream, sampleInfo *sample, MPEG_Data *videoData);

u8 byteAligned(u32 *bitsRead);
void rbspTrailingBits(u8 *data, u32 *bitsRead, u32 *bytesRead, u32 numberOfBits);

#endif // AVC_DECODE_H