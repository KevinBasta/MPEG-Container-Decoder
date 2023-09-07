
#include <stdlib.h>
#include <stdio.h>

#include <stdint.h>
#include "main.h"

#include "linkedlist.h"
#include "typesMPEG-4.h"
#include "typesStorage.h"

#include "bitUtility.h"
#include "endianUtility.h"
#include "printUtility.h"

#include "MPEG-4Parse.h"
#include "MPEG-4Decode.h"
#include "AVCDecode.h"

//https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/QTFFChap2/qtff2.html#//apple_ref/doc/uid/TP40000939-CH204-25680
//the following child atoms are required: sample description, sample size, sample to chunk, and chunk offset
//If the sync sample atom is not present, all samples are implicitly sync samples.

/*
Child boxes of STBL (Sample Table Atom). These define Samples and Chunks in the file.

*/
void stsdParseBox(box *stsdBox, MPEG_Data *videoData) { //sample description required
    u32 boxDataSize = stsdBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = stsdBox->boxData;

    u32 bytesRead = 0;

    u8 *version = referenceNBytes(1, boxData, &bytesRead);
    u8 *flags = referenceNBytes(3, boxData, &bytesRead);
    u8 *numberOfEntries = referenceNBytes(4, boxData, &bytesRead);
    u32 numberOfEntriesInt = bigEndianU8ArrToLittleEndianU32(numberOfEntries);
    printf("entries numb: %d\n", numberOfEntriesInt);
    
    linkedList *sampleDescriptionsLL = linkedlistInit();

    for (u32 i = 0; i < numberOfEntriesInt; i++) { 
        // General Structure Of A Sample Description        
        u32 sampleDescriptionSizeInt = bigEndianU8ArrToLittleEndianU32(referenceNBytes(4, boxData, &bytesRead));
        u32 absoluteEndOfThisSampleDescription = bytesRead + sampleDescriptionSizeInt - 4; // 4 for sampleDescriptionSize
        
        // data format indicates the type of compression that was used to compress 
        // the image data, or the color space representation of uncompressed video data
        u8 *dataFormat = referenceNBytes(4, boxData, &bytesRead);
        
        u8 *reserved = referenceNBytes(6, boxData, &bytesRead);
        u16 dataReferenceIndexInt = bigEndianU8ArrToLittleEndianU16(referenceNBytes(2, boxData, &bytesRead)); 

        printNBytes(dataFormat, 4, "data format: ", "\n");

        // The following fields assume that this stsd box belongs to a video trak 
        u8 *version         = referenceNBytes(2, boxData, &bytesRead);
        u8 *revisionLevel   = referenceNBytes(2, boxData, &bytesRead);
        u8 *vendor          = referenceNBytes(4, boxData, &bytesRead);
        
        i32 temporalQuality = bigEndianU8ArrToLittleEndianI32(referenceNBytes(4, boxData, &bytesRead));
        i32 spatialQuality  = bigEndianU8ArrToLittleEndianI32(referenceNBytes(4, boxData, &bytesRead));
        u16 width           = bigEndianU8ArrToLittleEndianU16(referenceNBytes(2, boxData, &bytesRead));
        u16 height          = bigEndianU8ArrToLittleEndianU16(referenceNBytes(2, boxData, &bytesRead));
        printf("t s w h %d %d %d %d\n", temporalQuality, spatialQuality, width, height);

        u8 *horizontalResolution = referenceNBytes(4, boxData, &bytesRead);
        u8 *verticalResolution   = referenceNBytes(4, boxData, &bytesRead);
        
        u8 *dataSize       = referenceNBytes(4, boxData, &bytesRead);
        u16 frameCount     = bigEndianU8ArrToLittleEndianU16(referenceNBytes(2, boxData, &bytesRead));
        u8 *compressorName = referenceNBytes(4, boxData, &bytesRead);
        printf("frame count %d\n", frameCount);
        printBits(compressorName, 4); 
        printNBytes(compressorName, 4, "compressor name: ", "\n");

        u16 depth        = bigEndianU8ArrToLittleEndianU16(referenceNBytes(2, boxData, &bytesRead));
        u16 colorTableID = bigEndianU8ArrToLittleEndianU16(referenceNBytes(2, boxData, &bytesRead));
        printf("depth %d\n", depth);
        printf("color id %d\n", colorTableID);
        
        printf("read: %d, end: %d\n", bytesRead, absoluteEndOfThisSampleDescription);
        
        //    Unspecified 28 bytes of all 0 bits not mentioned in spesification
        //    the last 4 bytes contains some unknown non-zero bits
        for (u32 i = 0; i < 7; i++) { 
            u8 *emptyFourBytes = referenceNBytes(4, boxData, &bytesRead);
            printBits(emptyFourBytes, 4);
            //printNBytes(emptyFourBytes, 4, "", "\n");
            //u32 *test = charToUnsignedInt(emptyFourBytes);
            //printf("%d\n", *test);
        }

        printf("read: %d, end: %d\n", bytesRead, absoluteEndOfThisSampleDescription);
        if (bytesRead != absoluteEndOfThisSampleDescription) {
            linkedList *stsdVideoExtentions = linkedlistInit();
            parseNestedChildBoxes(boxData, &bytesRead, absoluteEndOfThisSampleDescription, stsdVideoExtentions);
            linkedlistPrintAllBoxes(stsdVideoExtentions);

            printf("=============== avcc =================\n");
            box *avccBox = linkedlistGetBox(stsdVideoExtentions, "avcC");
            avccParseBox(avccBox, videoData);
            printf("=============== pasp =================\n");
            box *paspBox = linkedlistGetBox(stsdVideoExtentions, "pasp");
            paspParseBox(paspBox, videoData);
            printf("=============== btrt =================\n");
            box *btrtBox = linkedlistGetBox(stsdVideoExtentions, "btrt");
            btrtParseBox(btrtBox, videoData);
            printf("=============== colr =================\n");
            box *colrBox = linkedlistGetBox(stsdVideoExtentions, "colr");
            colrParseBox(colrBox, videoData);

        }
    }

}

void avccParseBox(box *avccBox, MPEG_Data *videoData) {
    if (avccBox == NULL) { 
        return;
    }

    AVC_Data *avcData = (AVC_Data*) malloc(sizeof(AVC_Data));

    u32 boxDataSize = avccBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = avccBox->boxData;

    u32 bytesRead = 0;

    u8 *version              = referenceNBytes(1, boxData, &bytesRead);
    u8 *profileIndication    = referenceNBytes(1, boxData, &bytesRead);
    u8 *profileCompatibility = referenceNBytes(1, boxData, &bytesRead);
    u8 *levelIndication      = referenceNBytes(1, boxData, &bytesRead);

    u8 *reservedAndLengthSizeMinusOne = copyNBytes(1, boxData, &bytesRead);
    u8 reservedOne      = getNBits(0, 5, *reservedAndLengthSizeMinusOne);
    u8 lengthSizeMinus1 = getNBits(6, 7, *reservedAndLengthSizeMinusOne);
    printBits(reservedAndLengthSizeMinusOne, 1);
    printf("length %u\n", lengthSizeMinus1);
    free(reservedAndLengthSizeMinusOne);

    // Seq Parameter Sets //

    u8 *reservedAndNumOfSequenceParameterSets = copyNBytes(1, boxData, &bytesRead);
    u8 reservedTwo                  = getNBits(0, 2, *reservedAndNumOfSequenceParameterSets);
    u8 numOfSequenceParameterSets   = getNBits(3, 7, *reservedAndNumOfSequenceParameterSets);
    free(reservedAndNumOfSequenceParameterSets);
    
    seqParameterSetTable *seqParamTable = malloc(sizeof(seqParameterSetTable));
    seqParamTable->numberOfEntries      = numOfSequenceParameterSets;
    seqParamTable->seqParameterSetArr   = (seqParameterSet**)   malloc(sizeof(seqParameterSet*) * numOfSequenceParameterSets);
    seqParamTable->seqParameterSetIdArr = (u32*)                malloc(sizeof(u32) * numOfSequenceParameterSets);

    for (u8 i = 0; i < numOfSequenceParameterSets; i++) { 
        u16 sequenceParameterSetLengthInt       = bigEndianU8ArrToLittleEndianU16(referenceNBytes(2, boxData, &bytesRead)); 
        u8 *sequenceParameterSetNALUnit         = referenceNBytes(sequenceParameterSetLengthInt, boxData, &bytesRead);
    
        seqParamTable->seqParameterSetArr[i]    = seqParameterSetRbspDecode(sequenceParameterSetLengthInt, sequenceParameterSetNALUnit);
        seqParamTable->seqParameterSetIdArr[i]  = seqParamTable->seqParameterSetArr[i]->seqParameterSetId;
    }


    // Pic Parameter Sets //
    
    u8 numOfPictureParameterSetsInt     =  bigEndianU8ArrToLittleEndianU8(referenceNBytes(1, boxData, &bytesRead));    
    
    picParameterSetTable *picParamTable = malloc(sizeof(picParameterSetTable));
    picParamTable->numberOfEntries      = numOfPictureParameterSetsInt;
    picParamTable->picParameterSetArr   = (picParameterSet**)   malloc(sizeof(picParameterSet*) * numOfPictureParameterSetsInt);
    picParamTable->picParameterSetIdArr = (u32*)                malloc(sizeof(u32) * numOfPictureParameterSetsInt);

    for (u8 i = 0; i < numOfPictureParameterSetsInt; i++) {
        u16 pictureParameterSetLengthInt    = bigEndianU8ArrToLittleEndianU16(referenceNBytes(2, boxData, &bytesRead));
        u8 *picutreParameterSetNALUnit      = referenceNBytes(pictureParameterSetLengthInt, boxData, &bytesRead);
        
        picParamTable->picParameterSetArr[i]    = picParameterSetRbspDecode(pictureParameterSetLengthInt, picutreParameterSetNALUnit);
        picParamTable->picParameterSetIdArr[i]  = picParamTable->picParameterSetArr[i]->picParameterSetId; 
    }
    
    avcData->lengthSizeMinus1 = lengthSizeMinus1;
    avcData->seqParamTable = seqParamTable;
    avcData->picParamTable = picParamTable;

    videoData->avcData = avcData;
    printf("bytes read: %d  \t box data size: %d\n", bytesRead, boxDataSize);
}


/**
 * @brief required for uncompressed Y'CbCr data formates. mpas the numerical values of pixels in 
 * file to a common representation of color in which images can be correctly compared, combined, and
 * displayed. The common representation is the CIE XYZ tristimulus values.
 * @param colrBox 
 * @note if this box and a gamma box are both defined, then ignore the gamma box
 */
void colrParseBox(box *colrBox, MPEG_Data *videoData) { 
    if (colrBox == NULL) { 
        return;
    }

    u32 boxDataSize = colrBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = colrBox->boxData;
    
    u32 bytesRead = 0;
    
    u8 *colorParameterType = referenceNBytes(4, boxData, &bytesRead);
    printNBytes(colorParameterType, 4, "type: ", "\n");
    
    u16 primariesIndex        = bigEndianU8ArrToLittleEndianU16(referenceNBytes(2, boxData, &bytesRead));
    u16 transferFunctionIndex = bigEndianU8ArrToLittleEndianU16(referenceNBytes(2, boxData, &bytesRead));
    u16 matrixIndex           = bigEndianU8ArrToLittleEndianU16(referenceNBytes(2, boxData, &bytesRead));
    printf("%d %d %d\n", primariesIndex, transferFunctionIndex, matrixIndex);
}

/**
 * @brief height to width rattio of pixels found in the video sample. 
 * required when non-square pixels are used.
 * @param paspBox 
 */
void paspParseBox(box *paspBox, MPEG_Data *videoData) { 
    if (paspBox == NULL) { 
        return;
    }

    u32 boxDataSize = paspBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = paspBox->boxData;

    u32 bytesRead = 0;

    u8 *hSpacing = referenceNBytes(4, boxData, &bytesRead);
    u8 *vSpacing = referenceNBytes(4, boxData, &bytesRead);

    u32 hSpacingInt = bigEndianCharToLittleEndianUnsignedInt(hSpacing);
    u32 vSpacingInt = bigEndianCharToLittleEndianUnsignedInt(vSpacing);

    printf("%d %d\n", hSpacingInt, vSpacingInt);
}


void btrtParseBox(box *btrtBox, MPEG_Data *videoData) { 
    if (btrtBox == NULL) { 
        return;
    }

    u32 boxDataSize = btrtBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = btrtBox->boxData;

    u32 bytesRead = 0;

    u8 *bufferSize = referenceNBytes(4, boxData, &bytesRead);
    u8 *maxBitRate = referenceNBytes(4, boxData, &bytesRead);
    u8 *averageBitRate = referenceNBytes(4, boxData, &bytesRead);
    
    u32 bufferSizeInt = bigEndianCharToLittleEndianUnsignedInt(bufferSize);
    u32 maxBitRateInt = bigEndianCharToLittleEndianUnsignedInt(maxBitRate);
    u32 averageBitRateInt = bigEndianCharToLittleEndianUnsignedInt(averageBitRate);

    printf("%d %d %d\n", bufferSizeInt, maxBitRateInt, averageBitRateInt);
}


/**
 * @brief sample size. REQUIRED.
 * to get size of each sample in media
 * @note path: moov->trak->mdia->minf->stbl->stsz
 * @param stszBox       -   the box
 * @param videoData     -   to store the following three things: 
 * sampleSizeInt, sampleSizeTable, and numberOfEntriesInt.
 * sampleSizeInt: if 0 then indicates that sample size table contains an entry 
 * for each sample. if non-zero then indicated the size of all samples.
 * sampleSizeTable: a table with an entry for each sample indicating 
 * it's size in bytes.
 * numberOfEntriesInt: equivalent to number of samples in media trak.
 */
void stszParseBox(box *stszBox, MPEG_Data *videoData) { //sample size required
    u32 boxDataSize = stszBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = stszBox->boxData;

    u32 bytesRead = 0;

    u8 *version = referenceNBytes(1, boxData, &bytesRead);
    u8 *flags = referenceNBytes(3, boxData, &bytesRead);

    u8 *sampleSize = referenceNBytes(4, boxData, &bytesRead);
    u8 *numberOfEntries = referenceNBytes(4, boxData, &bytesRead);
    
    // If all the samples are the same size, this field contains that size value. 
    // If this field is set to 0, then the samples have different sizes, and those 
    // sizes are stored in the sample size table.
    u32 sampleSizeInt = bigEndianCharToLittleEndianUnsignedInt(sampleSize);

    // equivalent to the number of samples in the media
    u32 numberOfEntriesInt = bigEndianCharToLittleEndianUnsignedInt(numberOfEntries);

    //printf("%d\n", sampleSizeInt);
    //printf("%d\n", numberOfEntriesInt);
    
    sampleSizeTable *table = (sampleSizeTable*) calloc(numberOfEntriesInt, sizeof(sampleSizeTable));
    u32 *sizeArr = (u32*) calloc(numberOfEntriesInt, sizeof(u32));
    table->totalEntries = numberOfEntriesInt;
    table->sampleSizeDefault = sampleSizeInt;
    table->sizeArr = sizeArr;

    for (i32 i = 0; i < numberOfEntriesInt; i++) { 
        u8 *size = referenceNBytes(4, boxData, &bytesRead);
        u32 sizeInt = bigEndianCharToLittleEndianUnsignedInt(size);

        sizeArr[i] = sizeInt;
        //printf("%d: %u\n", i + 1, sizeInt);
    }

    videoData->sampleSizeTable = table;
    videoData->numberOfSamples = numberOfEntriesInt;
}


/**
 * @brief sample to chunk. REQUIRED.
 * A chunk contains one or more samples. 
 * Contains a table that maps samples to their chunks.
 * Each table entry corresponds to a set of consecutive chunks, each of which contains the same number of samples. 
 * Furthermore, each of the samples in these chunks must use the same sample description. Whenever the number of 
 * samples per chunk or the sample description changes, you must create a new table entry. If all the chunks have 
 * the same number of samples per chunk and use the same sample description, this table has one entry.
 * @note path: moov->trak->mdia->minf->stbl->stsc
 * @param stscBox       -   the box
 * @param *videoData    -   to store sample-to-chunk table
 */
void stscParseBox(box *stscBox, MPEG_Data *videoData) { //sample to chunk required
    u32 boxDataSize = stscBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = stscBox->boxData;

    u32 bytesRead = 0;

    u8 *version = referenceNBytes(1, boxData, &bytesRead);
    u8 *flags = referenceNBytes(3, boxData, &bytesRead);
    u8 *numberOfEntries = referenceNBytes(4, boxData, &bytesRead);
    u32 numberOfEntriesInt = bigEndianCharToLittleEndianUnsignedInt(numberOfEntries);
    // DEBUG printf("%d\n", *numberOfEntriesInt);

    sampleToChunkTable *table = (sampleToChunkTable*) malloc(sizeof(sampleToChunkTable));
    u32 *firstChunkArr = (u32*) calloc(numberOfEntriesInt, sizeof(u32));
    u32 *samplesPerChunkArr = (u32*) calloc(numberOfEntriesInt, sizeof(u32));
    u32 *sampleDescriptionIdArr = (u32*) calloc(numberOfEntriesInt, sizeof(u32));
    
    table->totalEntries = numberOfEntriesInt;
    table->firstChunkArr = firstChunkArr;
    table->samplesPerChunkArr = samplesPerChunkArr;
    table->sampleDescriptionIdArr = sampleDescriptionIdArr;

    for (i32 i = 0; i < numberOfEntriesInt; i++) { 
        // first chunk refers to the index number of the chunk 
        // identifying a range from this first index number to the 
        // next table entry's first chunk index value, or the end of the file
        // calculated by the number of samples per chunk
        u8 *firstChunk = referenceNBytes(4, boxData, &bytesRead); 
        u8 *samplesPerChunk = referenceNBytes(4, boxData, &bytesRead); 
        u8 *sampleDescriptionId = referenceNBytes(4, boxData, &bytesRead);

        u32 firstChunkInt = bigEndianCharToLittleEndianUnsignedInt(firstChunk); 
        u32 samplesPerChunkInt = bigEndianCharToLittleEndianUnsignedInt(samplesPerChunk); 
        u32 sampleDescriptionIdInt = bigEndianCharToLittleEndianUnsignedInt(sampleDescriptionId); 
        
        firstChunkArr[i] = firstChunkInt;
        samplesPerChunkArr[i] = samplesPerChunkInt;
        sampleDescriptionIdArr[i] = sampleDescriptionIdInt;

        // DEBUG printf("%d \t\t %d \t\t %d \n", firstChunkInt, samplesPerChunkInt, sampleDescriptionIdInt);
    }

    videoData->sampleToChunkTable = table;
}


/**
 * @brief chunk offset. REQUIRED.
 * gives location of chunk data in media as file offsets, not offsets within
 * any atom. 
 * @note path: moov->trak->mdia->minf->stbl->stco
 * @param stcoBox       -   the box
 * @param videoData     -   to store chunk offset table
 */
void stcoParseBox(box *stcoBox, MPEG_Data *videoData) { //chunk offset required
    u32 boxDataSize = stcoBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = stcoBox->boxData;

    u32 bytesRead = 0;

    u8 *version = referenceNBytes(1, boxData, &bytesRead);
    u8 *flags = referenceNBytes(3, boxData, &bytesRead);
    u8 *numberOfEntries = referenceNBytes(4, boxData, &bytesRead);
    u32 numberOfEntriesInt = bigEndianCharToLittleEndianUnsignedInt(numberOfEntries);
    // DEBUG printf("%d\n", numberOfEntriesInt);
    
    chunkOffsetTable *table = (chunkOffsetTable*) malloc(sizeof(chunkOffsetTable));
    u32 *offsetArr = (u32*) calloc(numberOfEntriesInt, sizeof(u32));
    table->totalEntries = numberOfEntriesInt;
    table->offsetArr = offsetArr;

    for (i32 i = 0; i < numberOfEntriesInt; i++) {
        u8 *offset = referenceNBytes(4, boxData, &bytesRead);
        u32 offsetInt = bigEndianCharToLittleEndianUnsignedInt(offset);

        offsetArr[i] = offsetInt;
        // DEBUG printf("%d: %d\n", i, offsetInt);
    }

    videoData->chunkOffsetTable = table;
}


/**
 * @brief time to sample. 
 * stores duration information of media's samples.
 * the time-to-sample table can be used to map from media time to 
 * corresponding data sample.
 * @note path: moov->trak->mdia->minf->stbl->stts
 * @param sttsBox       -   the box
 * @param videoData     -   to store time-to-sample table
 */
void sttsParseBox(box *sttsBox, MPEG_Data *videoData) { //time to sample
    u32 boxDataSize = sttsBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = sttsBox->boxData;

    u32 bytesRead = 0;

    u8 *version = referenceNBytes(1, boxData, &bytesRead);
    u8 *flags = referenceNBytes(3, boxData, &bytesRead);
    u8 *numberOfEntries = referenceNBytes(4, boxData, &bytesRead);
    u32 numberOfEntriesInt = bigEndianCharToLittleEndianUnsignedInt(numberOfEntries); // for compressed
    u32 numberOfSamples = videoData->numberOfSamples; // for uncompressed
    // DEBUG printf("%d\n", *numberOfEntriesInt);

    // u32 numberOfSamples = 0; Done without accumulator in stsz


    timeToSampleTableCompressed *compressedTable = (timeToSampleTableCompressed*) malloc(sizeof(timeToSampleTableCompressed));
    u32 *compressedSampleCountArr = (u32*) calloc(numberOfEntriesInt, sizeof(u32));
    u32 *compressedSampleDurationArr = (u32*) calloc(numberOfEntriesInt, sizeof(u32));
    compressedTable->totalEntries = numberOfEntriesInt;
    compressedTable->sampleCountArr = compressedSampleCountArr;
    compressedTable->sampleDurationArr = compressedSampleDurationArr;
    
    timeToSampleTable *uncompressedTable = (timeToSampleTable*) malloc(sizeof(timeToSampleTable));
    u32 *uncompressedSampleDurationArr = (u32*) calloc(numberOfSamples, sizeof(u32));
    u32 *uncompressedSampleDeltaArr = (u32*) calloc(numberOfSamples, sizeof(u32));
    uncompressedTable->totalEntries = numberOfSamples;
    uncompressedTable->sampleDurationArr = uncompressedSampleDurationArr;
    uncompressedTable->sampleDeltaArr = uncompressedSampleDeltaArr;

    u32 uncompressedIterator = 0;
    u32 uncompressedDeltaAccumulator = 0;

    for (u32 i = 0; i < numberOfEntriesInt; i++) { 
        u8 *sampleCount = referenceNBytes(4, boxData, &bytesRead);
        u8 *sampleDuration = referenceNBytes(4, boxData, &bytesRead);
        
        u32 sampleCountInt = bigEndianCharToLittleEndianUnsignedInt(sampleCount);
        u32 sampleDurationInt = bigEndianCharToLittleEndianUnsignedInt(sampleDuration);
        
        // numberOfSamples += sampleCountInt;
        for (u32 j = 0; j < sampleCountInt; j++) { 
            uncompressedSampleDurationArr[uncompressedIterator] = sampleDurationInt;
            uncompressedSampleDeltaArr[uncompressedIterator] = uncompressedDeltaAccumulator;
            // printf("%d %d\n", uncompressedIterator, uncompressedSampleDeltaArr[uncompressedIterator]);
            
            uncompressedDeltaAccumulator += sampleDurationInt;
            uncompressedIterator++;
        }

        compressedSampleCountArr[i] = sampleCountInt;
        compressedSampleDurationArr[i] = sampleDurationInt;

    }

    videoData->timeToSampleTableCompressed = compressedTable;
    videoData->timeToSampleTable = uncompressedTable;
    // videoData->numberOfSamples = numberOfSamples; 
    // DEBUG printf("%d %d\n", videoData->timeToSampleTable[0]->sampleCount, videoData->timeToSampleTable[0]->sampleDuration);
}


/**
 * @brief sync sample.
 * https://wiki.multimedia.cx/index.php/QuickTime_container#stss
 * @param stssBox 
 * @param videoData 
 */
void stssParseBox(box *stssBox, MPEG_Data *videoData) { //sync sample
    if (stssBox == NULL) { 
        videoData->syncSampleTable = NULL;
        return;
    }

    u32 boxDataSize = stssBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = stssBox->boxData;

    u32 bytesRead = 0;

    u8 *version = referenceNBytes(1, boxData, &bytesRead);
    u8 *flags = referenceNBytes(3, boxData, &bytesRead);
    u8 *numberOfEntries = referenceNBytes(4, boxData, &bytesRead);
    u32 numberOfEntriesInt = bigEndianCharToLittleEndianUnsignedInt(numberOfEntries);
    // DEBUG printf("%d\n", numberOfEntriesInt);

    syncSampleTable *table = (syncSampleTable*) malloc(sizeof(syncSampleTable));
    u32 *sampleNumberArr = (u32*) calloc(numberOfEntriesInt, sizeof(u32));
    table->totalEntries = numberOfEntriesInt;
    table->sampleNumberArr = sampleNumberArr;


    for (i32 i = 0; i < numberOfEntriesInt; i++) { 
        u8 *entry = referenceNBytes(4, boxData, &bytesRead);
        u32 entryInt = bigEndianCharToLittleEndianUnsignedInt(entry);
        
        sampleNumberArr[i] = entryInt;
        // DEBUG printf("%d\n", entryInt);
    }

    videoData->syncSampleTable = table;
}


/**
 * @brief composition offset.
 * Video samples in encoded formats have a decode order and presentation order.
 * presentation order == composition order == display order. 
 * This box is used when there are out of order video samples.
 * If the decode and presentation orders are the same, this box will 
 * not be present.
 * If video samples are stored out of presenation order, this box 
 * contains the time of presentation as a delta on sample by sample basis.
 * DisplayTime = MediaTime + DisplayTimeDelta
 * @param cttsBox       -   the box
 * @param videoData     -   to store compositionOffsetTable
 */
void cttsParseBox(box *cttsBox, MPEG_Data *videoData) { //composition offset
    if (cttsBox == NULL) { 
        videoData->compositionOffsetTableCompressed = NULL;
        videoData->compositionOffsetTable = NULL;
        return;
    }
    
    u32 boxDataSize = cttsBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = cttsBox->boxData;

    u32 bytesRead = 0;

    u8 *version = referenceNBytes(1, boxData, &bytesRead);
    u8 *flags = referenceNBytes(3, boxData, &bytesRead);
    u8 *numberOfEntries = referenceNBytes(4, boxData, &bytesRead);
    u32 numberOfEntriesInt = bigEndianCharToLittleEndianUnsignedInt(numberOfEntries); // for compressed
    u32 numberOfSamples = videoData->numberOfSamples; // for uncompressed
    //printf("%d\n", numberOfEntriesInt);
    //printf("Sample Count \t\t Composition Offset\n");

    // creating three different tables
    compositionOffsetTableCompressed *compressedTable = (compositionOffsetTableCompressed*) malloc(sizeof(compositionOffsetTableCompressed));
    u32 *compressedSampleCountArr = (u32*) calloc(numberOfEntriesInt, sizeof(u32));
    u32 *compressedCompositionOffsetArr = (u32*) calloc(numberOfEntriesInt, sizeof(u32));
    compressedTable->totalEntries = numberOfEntriesInt;
    compressedTable->sampleCountArr = compressedSampleCountArr;
    compressedTable->compositionOffsetArr = compressedCompositionOffsetArr;
    
    compositionOffsetTable *uncompressedTable = (compositionOffsetTable*) malloc(sizeof(compositionOffsetTable));
    i32 *uncompressedCompositionOffsetArr = (i32*) calloc(numberOfSamples, sizeof(i32));
    uncompressedTable->totalEntries = numberOfSamples;
    uncompressedTable->compositionOffsetArr = uncompressedCompositionOffsetArr;
    
    u32 uncompressedIterator = 0;

    for (i32 i = 0; i < numberOfEntriesInt; i++) { 
        u8 *sampleCount = referenceNBytes(4, boxData, &bytesRead);
        u8 *compositionOffset = referenceNBytes(4, boxData, &bytesRead);
        
        u32 sampleCountInt = bigEndianCharToLittleEndianUnsignedInt(sampleCount);
        i32 compositionOffsetInt = bigEndianCharToLittleEndianInt(compositionOffset);
        
        for (u32 j = 0; j < sampleCountInt; j++) { 
            uncompressedCompositionOffsetArr[uncompressedIterator] = compositionOffsetInt;
            
            uncompressedIterator++;
        }

        compressedSampleCountArr[i] = sampleCountInt;
        compressedCompositionOffsetArr[i] = compositionOffsetInt;

        // printf("%d \t\t %d\n", sampleCountInt, compositionOffsetInt);
    }

    videoData->compositionOffsetTableCompressed = compressedTable;
    videoData->compositionOffsetTable = uncompressedTable;
}


/**
 * @brief theoretically supposed to contain instructions 
 * for how to access the media's data in the data references table.
 * @note path: moov->trak->mdia->minf->dinf->dref
 * @param drefBox       -   the box
 * @param *videoData    -   for storing the data reference table
 */
void drefParseBox(box *drefBox, MPEG_Data *videoData) { 
    u32 boxDataSize = drefBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = drefBox->boxData;

    u32 bytesRead = 0;
    
    u8 *version = referenceNBytes(1, boxData, &bytesRead);
    u8 *flags = referenceNBytes(3, boxData, &bytesRead);
    u8 *numberOfEntries = referenceNBytes(4, boxData, &bytesRead);
    u32 numberOfEntriesInt = bigEndianCharToLittleEndianUnsignedInt(numberOfEntries);
    printf("entries: %d\n", numberOfEntriesInt);

    dataReferenceTableEntry **dataReferenceTable = (dataReferenceTableEntry**) calloc(numberOfEntriesInt + 1, sizeof(dataReferenceTableEntry*)); 
    dataReferenceTable[numberOfEntriesInt] = NULL;
    
    for (i32 i = 0; i < numberOfEntriesInt; i++) { 
        dataReferenceTableEntry *dataReferenceEntry = (dataReferenceTableEntry*) malloc(sizeof(dataReferenceTableEntry));
        u8 *dataReferenceSize = referenceNBytes(BOX_SIZE_SIZE, boxData, &bytesRead);
        dataReferenceEntry->size = bigEndianCharToLittleEndianUnsignedInt(dataReferenceSize);

        dataReferenceEntry->type = copyNBytes(BOX_TYPE_SIZE, boxData, &bytesRead);

        // flag is used to indicate that media's data is in same file
        dataReferenceEntry->version = copyNBytes(BOX_VERSION_SIZE, boxData, &bytesRead);
        dataReferenceEntry->flags = copyNBytes(BOX_FLAG_SIZE, boxData, &bytesRead);

        u32 dataReferenceDataSize = dataReferenceEntry->size - BOX_HEADER_SIZE - BOX_VERSION_SIZE - BOX_FLAG_SIZE;
        dataReferenceEntry->data = copyNBytes(dataReferenceDataSize, boxData, &bytesRead);

        
        dataReferenceTable[i] = dataReferenceEntry;
        
        printNBytes(dataReferenceEntry->type, BOX_TYPE_SIZE, "", "\n");
        printBits(dataReferenceEntry->data, dataReferenceDataSize);
        printBits(dataReferenceEntry->flags, 3);
    }

    videoData->dataReferenceTable = dataReferenceTable;
}


/**
 * @brief parses it's child box, deref. helps in interpreting the 
 * media's data.
 * @note path: moov->trak->mdia->minf->dinf
 * @param dinfBox       -   the box
 * @param *videoData    -   to pass to dref 
 */
void dinfParseBox(box *dinfBox, MPEG_Data *videoData) {
    u32 boxDataSize = dinfBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = dinfBox->boxData;

    u32 bytesRead = 0;

    box *drefBox = parseSingleNestedChildBox(boxData, &bytesRead);
    drefParseBox(drefBox, videoData);
}


/**
 * @brief unused box
 * @note path: moov->trak->mdia->minf->vmhd
 * @param vmhdBox 
 */
void vmhdParseBox(box *vmhdBox) { 
    u32 boxDataSize = vmhdBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = vmhdBox->boxData;

    u32 bytesRead;
    bytesRead = 0;

    u8 *version = referenceNBytes(1, boxData, &bytesRead);
    u8 *flags = referenceNBytes(3, boxData, &bytesRead);
    u8 *graphicsMode = referenceNBytes(2, boxData, &bytesRead);
    u8 *opcolorOne = referenceNBytes(2, boxData, &bytesRead); //Red
    u8 *opcolorTwo = referenceNBytes(2, boxData, &bytesRead); //Green
    u8 *opcolorThree = referenceNBytes(2, boxData, &bytesRead); //Blue
}


/**
 * @brief identifies if the current trak is Video or Audio
 * @note path: moov->trak->mdia->hdlr
 * @param hdlrBox   -   the box
 * @return the componentSubtype that determins if this box's parent trak atom is 
 * of type video or not. vide return descides if should parse this trak further.
 */
u8 *hdlrParseBox(box *hdlrBox) { 
    u32 boxDataSize = hdlrBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = hdlrBox->boxData;

    u32 bytesRead = 0;

    u8 *version = referenceNBytes(1, boxData, &bytesRead);
    u8 *flags = referenceNBytes(3, boxData, &bytesRead);

    // Start Of Used Values //
    u8 *componentType = referenceNBytes(4, boxData, &bytesRead);
    u8 *componentSubtype = referenceNBytes(4, boxData, &bytesRead); // check if type is vide !IMPORTANT
    // End Of Used Values //
    printNBytes(componentType, 4, "type ", "\n");
    printNBytes(componentSubtype, 4, "sub type ", "\n");

    u8 *componentManufacturer = referenceNBytes(4, boxData, &bytesRead);
    u8 *componentFlags = referenceNBytes(4, boxData, &bytesRead);
    u8 *componentFlagsMask = referenceNBytes(4, boxData, &bytesRead);

    i32 componentNameLength = boxDataSize - bytesRead;
    u8 *componentName = referenceNBytes(componentNameLength, boxData, &bytesRead);

    return componentSubtype;
}


/**
 * @brief gives characteristics of specific media
 * @note path: moov->trak->mdia->mdhd
 * @param mdhdBox   -   the box
 * @param videoData -   struct to store VIDEO timescale and duration
 */
void mdhdParseBox(box *mdhdBox, MPEG_Data *videoData) { 
    u32 boxDataSize = mdhdBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = mdhdBox->boxData;

    u32 bytesRead = 0;

    u8 *version = referenceNBytes(1, boxData, &bytesRead);
    u8 *flags = referenceNBytes(3, boxData, &bytesRead);
    u8 *creationTime = referenceNBytes(4, boxData, &bytesRead);
    u8 *modificationTime = referenceNBytes(4, boxData, &bytesRead);

    // Start Of Used Values //
    u8 *timeScale = referenceNBytes(4, boxData, &bytesRead);
    u8 *duration = referenceNBytes(4, boxData, &bytesRead);
    
    u32 timeScaleInt = bigEndianCharToLittleEndianUnsignedInt(timeScale);
    u32 durationInt = bigEndianCharToLittleEndianUnsignedInt(duration);
    
    videoData->mdhdTimeScale = timeScaleInt;
    videoData->mdhdDuration = durationInt;
    // End Of Used Values //
    printf("%d %d\n", timeScaleInt, durationInt);

    
    u8 *language = referenceNBytes(2, boxData, &bytesRead);
    u8 *quality = referenceNBytes(2, boxData, &bytesRead);

    //printBits(language, 2);
    //printBits(quality, 2);
}


/**
 * @brief parses elst table
 * @note path: moov->trak->edts->elst
 * @param elstBox   -   the box
 * @param videoData -   struct to store elstTable in
 */
void elstParseBox(box *elstBox, MPEG_Data *videoData) { 
    u32 boxDataSize = elstBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = elstBox->boxData;

    u32 bytesRead = 0;

    u8 *version = referenceNBytes(1, boxData, &bytesRead);
    u8 *flags = referenceNBytes(3, boxData, &bytesRead);
    u8 *numberOfEntries = referenceNBytes(4, boxData, &bytesRead);
    u32 numberOfEntriesInt = bigEndianCharToLittleEndianUnsignedInt(numberOfEntries);
    printf("number of edit list entries: %u\n", numberOfEntriesInt);

    // there can be multiple table entries in for example the video stops in
    // the middle of the movie and then continues later on
    
    //table *elstTable = (table*) malloc(sizeof(table));

    elstTable *table = (elstTable*) malloc(sizeof(elstTable));
    u32 *trackDurationArr = (u32*) calloc(numberOfEntriesInt, sizeof(u32));
    u32 *mediaTimeArr = (u32*) calloc(numberOfEntriesInt, sizeof(u32));
    u32 *mediaRateArr = (u32*) calloc(numberOfEntriesInt, sizeof(u32));
    table->totalEntries = numberOfEntriesInt;
    table->trackDurationArr = trackDurationArr;
    table->mediaTimeArr = mediaTimeArr;
    table->mediaRateArr = mediaRateArr;

    for (i32 i = 0; i < numberOfEntriesInt; i++) { 
        u8 *trackDuration = referenceNBytes(4, boxData, &bytesRead);
        u8 *mediaTime = referenceNBytes(4, boxData, &bytesRead);
        u8 *mediaRate = referenceNBytes(4, boxData, &bytesRead);

        u32 trackDurationInt = bigEndianCharToLittleEndianUnsignedInt(trackDuration);
        i32 mediaTimeInt = bigEndianCharToLittleEndianInt(mediaTime);
        u32 mediaRateInt = bigEndianCharToLittleEndianUnsignedInt(mediaRate);
        // DEBUG printf("%d %d %d\n", trackDurationInt, mediaTimeInt, mediaRateInt);

        trackDurationArr[i] = trackDurationInt;
        mediaTimeArr[i] = mediaTimeInt;
        mediaRateArr[i] = mediaRateInt;
    }
    
    videoData->elstTable = table;


    /* { 
        i32 i = 0;
        while (elstTable[i] != NULL) { 
            printf("%d %d %d\n", *(elstTable[i]->trackDuration), *(elstTable[i]->mediaTime), *(elstTable[i]->mediaRate));
            i++;
        }
    } */
}


/**
 * @brief parses it's single elst child box
 * @note path: moov->trak->edts
 * @param edtsBox   -   the box
 * @param videoData -   just for passing to child
 */
void edtsParseBox(box *edtsBox, MPEG_Data *videoData) {
    //Note: If the edit atom or the edit list atom is missing, you can assume that the entire media is used by the track.
    if (edtsBox == NULL) { 
        videoData->elstTable = NULL;
        return;
    }
   
    u32 boxDataSize = edtsBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = edtsBox->boxData;

    u32 bytesRead = 0;

    box *elstBox = parseSingleNestedChildBox(boxData, &bytesRead);
    elstParseBox(elstBox, videoData);
}


/**
 * @brief gives duration of track in question. used to get duration of video.
 * @note path: moov->trak->tkhd
 * @param trakBox   -   the box
 * @param videoData -   struct to store TRACK duration
 */
void tkhdParseBox(box *trakBox, MPEG_Data *videoData) {
    u32 boxDataSize = trakBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = trakBox->boxData;

    u32 bytesRead = 0;

    u8 *version = referenceNBytes(1, boxData, &bytesRead);
    u8 *flags = referenceNBytes(3, boxData, &bytesRead);
    u8 *creationTime = referenceNBytes(4, boxData, &bytesRead);
    u8 *modificationTime = referenceNBytes(4, boxData, &bytesRead);
    u8 *trackId = referenceNBytes(4, boxData, &bytesRead);
    u8 *reservedOne = referenceNBytes(4, boxData, &bytesRead);

    // Start Of Used Values //
    u8 *duration = referenceNBytes(4, boxData, &bytesRead);
    u32 durationInt = bigEndianCharToLittleEndianUnsignedInt(duration);
    videoData->tkhdTrackDuration = durationInt;
    // End Of Used Values //
    printf("duration: %d\n", durationInt);


    u8 *reservedTwo = referenceNBytes(8, boxData, &bytesRead);
    u8 *layer = referenceNBytes(2, boxData, &bytesRead);
    u8 *alternateGroup = referenceNBytes(2, boxData, &bytesRead);
    u8 *volume = referenceNBytes(2, boxData, &bytesRead);
    u8 *reservedThree = referenceNBytes(2, boxData, &bytesRead);
    u8 *matrixStructure = referenceNBytes(36, boxData, &bytesRead);
    //printNBytes(matrixStructure, 36, "matrix: ", "\n");
    
    
    u8 *trackWidth = referenceNBytes(4, boxData, &bytesRead);
    //u32 *trackWidthInt = charToUnsignedInt(trackWidth);
    u8 *trackHeight = referenceNBytes(4, boxData, &bytesRead);
    //u32 *trackHeightInt = charToUnsignedInt(trackHeight);
    //printf("width: %d height: %d\n", *trackWidthInt, *trackHeightInt);



    printf("%d %d\n", bytesRead, boxDataSize);
    if (bytesRead == boxDataSize) {
        printf("read all\n");
    } else {
        printf("no\n");
    } 
}


/**
 * @brief gives a timescale and duration for the entire movie
 * @note path: moov->mvhd
 * @param mvhdBox   -   the box
 * @param videoData -   struct to store ENTIRE MOVIE timescale and duration
 */
void mvhdParseBox(box *mvhdBox, MPEG_Data *videoData) { 
    u32 boxDataSize = mvhdBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = mvhdBox->boxData;

    u32 bytesRead = 0;

    u8 *version = referenceNBytes(1, boxData, &bytesRead);
    u8 *flags = referenceNBytes(3, boxData, &bytesRead);
    u8 *creationTime = referenceNBytes(4, boxData, &bytesRead);
    u8 *modificationTime = referenceNBytes(4, boxData, &bytesRead);
    
    
    // Start Of Used Values //
    u8 *timeScale = referenceNBytes(4, boxData, &bytesRead);
    u32 timeScaleInt = bigEndianCharToLittleEndianUnsignedInt(timeScale);
    videoData->mvhdTimeScale = timeScaleInt;

    u8 *duration = referenceNBytes(4, boxData, &bytesRead);
    u32 durationInt = bigEndianCharToLittleEndianUnsignedInt(duration);
    videoData->mvhdDuration = durationInt;
    // End Of Used Values //

    printf("timescale as int: %u\n", timeScaleInt);
    printf("duration as int: %u\n", durationInt);



    //printNBytes(duration, 4, "duration: ", ".\n");
    //printf("duration/timescale %lf\n", (*durationInt / *timeScaleInt) / 60.0);
    //printHexNBytes(duration, 4);
    
    u8 *preferredRate = referenceNBytes(4, boxData, &bytesRead);
    u8 *preferredVolume = referenceNBytes(2, boxData, &bytesRead);
    u8 *reserved = referenceNBytes(10, boxData, &bytesRead);
    u8 *matrixStructure = referenceNBytes(36, boxData, &bytesRead);
    u8 *previewTime = referenceNBytes(4, boxData, &bytesRead);
    u8 *previewDuration = referenceNBytes(4, boxData, &bytesRead);
    u8 *posterTime = referenceNBytes(4, boxData, &bytesRead);
    u8 *selectionTime = referenceNBytes(4, boxData, &bytesRead);
    u8 *selectionDuration = referenceNBytes(4, boxData, &bytesRead);
    u8 *currentTime = referenceNBytes(4, boxData, &bytesRead);
    u8 *nextTrackId = referenceNBytes(4, boxData, &bytesRead);

    printf("%d %d\n", bytesRead, boxDataSize);
    if (bytesRead == boxDataSize) {
        printf("read all\n");
    } else {
        printf("no\n");
    } 
}





/**
 *  reads a ftyp box's major brand, minor version, and compatible brands
 *  @param *ftypBox:    a pointer to a ftyp box struct
 */
void ftypParseBox(box *ftypBox) { 
    u32 boxDataSize = ftypBox->boxSize - BOX_HEADER_SIZE;
    u8 *boxData = ftypBox->boxData;

    /*
        reading the ftyp fields
        majorBrand: 4 bytes the preferred brand or best use
        minorVersion: 4 bytes indicates the file format specification version
        compatibleBrands[]: an array of 4 byte compatible file formats
    */
    u32 bytesRead = 0;
    u8 *majorBrand = (u8*) malloc(4);
    for (i32 i = 0; i < 4; i++) {
        majorBrand[i] = boxData[bytesRead];
        bytesRead += 1;
    }
    printNBytes(majorBrand, 4, "", "\n");

    u8 *minorVersion = (u8*) malloc(4);
    for (i32 i = 0; i < 4; i++) {
        minorVersion[i] = boxData[bytesRead];
        bytesRead += 1;
    }
    printHexNBytes(minorVersion, 4);

    u32 compatibleBrandsSize = boxDataSize - (bytesRead + 1);
    u8 *compatibleBrands = (u8*) malloc(compatibleBrandsSize);
    
    i32 compatibleBrandsByte = 0;
    while (bytesRead < boxDataSize) {
        compatibleBrands[compatibleBrandsByte] = boxData[bytesRead];
        bytesRead += 1;
        compatibleBrandsByte += 1;
    }

    for (i32 i = 0; i < compatibleBrandsSize; i+=4) {
        printNBytes(&compatibleBrands[i], 4, "", "\n");
    }
}
