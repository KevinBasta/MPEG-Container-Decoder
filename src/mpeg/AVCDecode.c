
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <stdint.h>
#include "main.h"

#include "linkedlist.h"
#include "typesAVC.h"
#include "typesStorage.h"
#include "typesUtility.h"

#include "AVCMath.h"
#include "AVCDecode.h"
#include "bitUtility.h"
#include "endianUtility.h"
#include "printUtility.h"

void parseAVCSample(sampleInfo *sampleData, MPEG_Data *videoData) { 
    sampleData->mdatPointer = &(videoData->mdatBox->boxData[sampleData->sampleOffsetInMdat]);
    
    u32 bytesRead = sampleData->sampleOffsetInMdat;
    u32 sampleSize = sampleData->sampleSize;
    u32 unitLengthFieldSize = videoData->avcData->lengthSizeMinus1 + 1;
    // DEBUG printf("offset: %d\n", sampleData->sampleOffsetInMdat);
    int i;

    for (i = 0; i < sampleSize - 4;) {
        printf("i: %d\n", i);
        // since NALUnitLength can be 1, 2, or 4 bytes, will just store it in 4 byte int
        u8 *NALUnitLength    = referenceNBytes(unitLengthFieldSize, videoData->mdatBox->boxData, &bytesRead);
        // DEBUG printHexNBytes(&(videoData->mdatBox->boxData[sampleData->sampleOffsetInMdat]), 4);
        printf("bytes read: %d\n", bytesRead - sampleData->sampleOffsetInMdat);
        
        // generalized is requied here, spesific functions won't work because the byteNumb would be assumed
        u32 NALUnitFullLengthInt = bigEndianCharToLittleEndianGeneralized(NALUnitLength, unitLengthFieldSize); 
        u32 NALUnitDataLengthInt = NALUnitFullLengthInt - unitLengthFieldSize;

        u8 *NALDataStream          = referenceNBytes(NALUnitDataLengthInt, videoData->mdatBox->boxData, &bytesRead);
        parseNALUnit(NALUnitDataLengthInt, NALDataStream);

        i = i + unitLengthFieldSize + NALUnitDataLengthInt;

        // DEBUG printf("sample size: %7d nal unit length: %10u bytes read: %10u \n", sampleSize, NALUnitLengthInt, bytesRead);
    }
    printf("final i: %d\n", i);
    printf("sample Size: %d\n", sampleSize);
    printf("bytes read: %d\n", bytesRead - sampleData->sampleOffsetInMdat);
}


void parseNALUnits(NAL_Data *nalData) { 
    
}


NALUnitInfo *parseNALUnit(u32 NALUnitDataLength, u8 *NALDataStream) {
    NALUnitInfo *NALUnit = (NALUnitInfo*) malloc(sizeof(NALUnitInfo));

    u32 bytesRead = 0;
    u8 emulationPreventionByte = FALSE;

    u8 *zeroBitAndNALIdcAndUnitType = copyNBytes(1, NALDataStream, &bytesRead);
    u8 forbiddenZeroBit             = getNBits(0, 0, *zeroBitAndNALIdcAndUnitType);
    u8 NALRefIdc                    = getNBits(1, 2, *zeroBitAndNALIdcAndUnitType); 
    u8 NALUnitType                  = getNBits(3, 7, *zeroBitAndNALIdcAndUnitType);
    free(zeroBitAndNALIdcAndUnitType);
    //u32 test = bigEndianU8ArrToLittleEndianU32(zeroBitAndNALIdcAndUnitType);
    //printf("===========================%d\n", test);

    // DEBUG printf("nal ref id %d  nal unit type %d\n", NALRefIdc, NALUnitType);
    
    // very slow process
    // discard sample if emulation byte present?
    /* for (int i = 1; i < NALUnitDataLength; i++) { 
        if (i + 2 < NALUnitDataLength && bigEndianCharToLittleEndianGeneralized(checkNextNBytes(3, NALDataStream, bytesRead), 3) == 0x000003) { 
            emulationPreventionByte = TRUE;
            bytesRead += 2;
            i += 2;
            // DEBUG printBits(referenceNBytes(1, NALDataStream, &bytesRead), 1);
        } else { 
            bytesRead++;
        }
    } */
    
    NALUnit->NALRefIdc = NALRefIdc;
    NALUnit->NALUnitType = NALUnitType;
    NALUnit->NALUnitData = NALDataStream + 1;
    //printf("%X\n", NALDataStream);
    //printf("%X\n", NALDataStream + 1);
    
    if (emulationPreventionByte == TRUE) { 
        NALUnit->NALUnitDataLength = NALUnitDataLength - 1;
    } else { 
        NALUnit->NALUnitDataLength = NALUnitDataLength;
    }

    /* if (NALUnitType == 8) { 
        picParameterSetRbsp(NALUnitDataLength - 1, NALDataStream + 1);
    } else if (NALUnitType == 7) { 
        seqParameterSetRbsp(NALUnitDataLength - 1, NALDataStream + 1);
    } else {
        printf("nal unit type: %d\n", NALUnitType);
    } */

    return NALUnit;
}


picParameterSet *picParameterSetRbspDecode(u32 NALUnitDataLength, u8 *NALDataStream) { 
    NALUnitInfo *NALUnit = parseNALUnit(NALUnitDataLength, NALDataStream);
    
    u32 bitsRead = 0;
    u32 bytesRead = 0;

    u8 *data = NALUnit->NALUnitData;
    u32 dataLength = NALUnit->NALUnitDataLength;

    printf("===============\n");
    picParameterSet *pps = calloc(1, sizeof(picParameterSet));

    pps->picParameterSetId       = ue(data, &bitsRead, &bytesRead, dataLength);
    pps->seqParameterSetId       = ue(data, &bitsRead, &bytesRead, dataLength);
    pps->entropyCodingModeFlag   = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);
    pps->picOrderPresentFlag     = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);
    pps->numSliceGroupsMinus1    = ue(data, &bitsRead, &bytesRead, dataLength);
    
    printf("%d %d %d %d %d\n\n",pps->picParameterSetId, 
                                pps->seqParameterSetId, 
                                pps->entropyCodingModeFlag, 
                                pps->picOrderPresentFlag, 
                                pps->numSliceGroupsMinus1);

    
    pps->sliceGroupMapType;
    pps->runLengthMinus1            = NULL;
    pps->topLeft                    = NULL;
    pps->bottomRight                = NULL;
    pps->sliceGroupChangeDirectionFlag;
    pps->sliceGroupChangeRateMinus1;
    pps->picSizeInMapUnitsMinus1;
    pps->sliceGroupId               = NULL;

    if (pps->numSliceGroupsMinus1 > 0) { 
        pps->sliceGroupMapType = ue(data, &bitsRead, &bytesRead, dataLength);
        printf("slice group: %d\n", pps->sliceGroupMapType);
        
        if (pps->sliceGroupMapType == 0) 
        {

            pps->runLengthMinus1                = malloc(sizeof(u32) * (pps->numSliceGroupsMinus1 + 1));
            
            for (u32 iGroup = 0; iGroup <= pps->numSliceGroupsMinus1; iGroup++) { 
                pps->runLengthMinus1[iGroup]    = ue(data, &bitsRead, &bytesRead, dataLength);
            }

        } 
        else if (pps->sliceGroupMapType == 2) 
        {

            pps->topLeft                        = malloc(sizeof(u32) * pps->numSliceGroupsMinus1);
            pps->bottomRight                    = malloc(sizeof(u32) * pps->numSliceGroupsMinus1);

            for (u32 iGroup = 0; iGroup < pps->numSliceGroupsMinus1; iGroup++) { 
                pps->topLeft[iGroup]            = ue(data, &bitsRead, &bytesRead, dataLength);
                pps->bottomRight[iGroup]        = ue(data, &bitsRead, &bytesRead, dataLength);
            }

        }
        else if (pps->sliceGroupMapType == 3 || pps->sliceGroupMapType == 4 || pps->sliceGroupMapType == 5) 
        {   

            pps->sliceGroupChangeDirectionFlag  = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);
            pps->sliceGroupChangeRateMinus1     = ue(data, &bitsRead, &bytesRead, dataLength);

        }
        else if (pps->sliceGroupMapType == 6)
        {

            pps->picSizeInMapUnitsMinus1        = ue(data, &bitsRead, &bytesRead, dataLength); 
            pps->sliceGroupId                   = malloc(sizeof(u32) * (pps->picSizeInMapUnitsMinus1 + 1));
            
            u32 sliceGroupIdLength              = ceil( log2( pps->numSliceGroupsMinus1 + 1 ) );
            for (u32 i = 0; i <= pps->picSizeInMapUnitsMinus1; i++) { 
                pps->sliceGroupId[i]            = getUnsignedNBits(data, &bitsRead, &bytesRead, sliceGroupIdLength);
            }

        }
    }

    pps->numRefIdxl0ActiveMinus1                 = ue(data, &bitsRead, &bytesRead, dataLength);
    pps->numRefIdxl1ActiveMinus1                 = ue(data, &bitsRead, &bytesRead, dataLength);
    pps->weightedPredFlag                        = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);
    pps->weightedBipredIdc                       = getUnsignedNBits(data, &bitsRead, &bytesRead, 2);
    pps->picInitQpMinus26                        = se(data, &bitsRead, &bytesRead, dataLength);
    pps->picInitQsMinus26                        = se(data, &bitsRead, &bytesRead, dataLength);
    pps->chromaQpIndexOffset                     = se(data, &bitsRead, &bytesRead, dataLength);
    pps->deblockingFilterVariablesPresentFlag    = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);
    pps->constrainedIntraPredFlag                = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);
    pps->redundantPicCntPresentFlag              = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);
    pps->frameCroppingFlag                       = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);

    if (pps->frameCroppingFlag) { 
        pps->frameCropLeftOffset     = ue(data, &bitsRead, &bytesRead, dataLength);
        pps->frameCropRightOffset    = ue(data, &bitsRead, &bytesRead, dataLength);
        pps->frameCropTopOffset      = ue(data, &bitsRead, &bytesRead, dataLength);
        pps->frameCropBottomOffset   = ue(data, &bitsRead, &bytesRead, dataLength);
    }

    rbspTrailingBits(data, &bitsRead, &bytesRead, dataLength);

    printf("BITS READ %d\n", bitsRead);
    printf("TOTAL BITS %d\n", (dataLength * 8));
    printBits(data, dataLength);
    printf("===============\n");

    free(NALUnit);
    return pps;

    // to move to freeing function
    if (pps->runLengthMinus1 != NULL)
        free(pps->runLengthMinus1);

    if (pps->topLeft != NULL)
        free(pps->topLeft);

    if (pps->bottomRight != NULL)
        free(pps->bottomRight);

    if (pps->sliceGroupId != NULL)
        free(pps->sliceGroupId);
    
    free(pps);
}



seqParameterSet *seqParameterSetRbspDecode(u32 NALUnitDataLength, u8 *NALDataStream) { 
    NALUnitInfo *NALUnit = parseNALUnit(NALUnitDataLength, NALDataStream);

    u32 bitsRead = 0;
    u32 bytesRead = 0;

    u8 *data = NALUnit->NALUnitData;
    u32 dataLength = NALUnit->NALUnitDataLength;

    printf("===============\n");
    seqParameterSet *sps = calloc(1, sizeof(seqParameterSet));

    sps->profileIdc                          = getUnsignedNBits(data, &bitsRead, &bytesRead, 8);
    sps->levelIdc                            = getUnsignedNBits(data, &bitsRead, &bytesRead, 8);
    sps->moreThanOneSliceGroupAllowedFlag    = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);
    sps->arbitrarySliceOrderAllowedFlag      = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);
    sps->redundantPicturesAllowedFlag        = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);

    sps->seqParameterSetId                   = ue(data, &bitsRead, &bytesRead, dataLength);
    sps->log2MaxFrameNumMinus4               = ue(data, &bitsRead, &bytesRead, dataLength);
    sps->picOrderCntType                     = ue(data, &bitsRead, &bytesRead, dataLength);


    sps->log2MaxPicOrderCntLsbMinus4;
    sps->deltaPicOrderAlwaysZeroFlag;
    sps->offsetForNonRefPic;
    sps->offsetForTopToBottomField;
    sps->numRefFramesInPicOrderCntCycle;
    sps->offsetForRefFrame = NULL;
    if (sps->picOrderCntType == 0)
    { 
        sps->log2MaxPicOrderCntLsbMinus4     = se(data, &bitsRead, &bytesRead, dataLength);
    } 
    else if (sps->picOrderCntType == 1)
    { 
        sps->deltaPicOrderAlwaysZeroFlag    = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);
        sps->offsetForNonRefPic             = se(data, &bitsRead, &bytesRead, dataLength);
        sps->offsetForTopToBottomField      = se(data, &bitsRead, &bytesRead, dataLength);
        sps->numRefFramesInPicOrderCntCycle = ue(data, &bitsRead, &bytesRead, dataLength);

        sps->offsetForRefFrame              = (i32*) malloc(sizeof(i32) * sps->numRefFramesInPicOrderCntCycle);
        for (u32 i = 0; i < sps->numRefFramesInPicOrderCntCycle; i++) { 
            sps->offsetForRefFrame[i]       = se(data, &bitsRead, &bytesRead, dataLength);
        }
    }

    sps->numRefFrames                           = ue(data, &bitsRead, &bytesRead, dataLength); 
    sps->requiredFrameNumUpdateBehaviourFlag    = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);
    sps->picWidthInMbsMinus1                    = ue(data, &bitsRead, &bytesRead, dataLength); 
    sps->picHeightInMapUnitsMinus1              = ue(data, &bitsRead, &bytesRead, dataLength); 
    sps->frameMbsOnlyFlag                       = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);

    if (!(sps->frameMbsOnlyFlag)) { 
        sps->mbAdaptiveFrameFieldFlag           = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);
    }

    sps->direct8x8InferenceFlag                 = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);
    sps->vuiParametersPresentFlag               = getUnsignedNBits(data, &bitsRead, &bytesRead, 1);

    if (sps->vuiParametersPresentFlag) { 
        //vui_parameters();
        printf("vui parameters present\n");
    }

    rbspTrailingBits(data, &bitsRead, &bytesRead, dataLength);

    printf("BITS READ %d\n", bitsRead);
    printf("TOTAL BITS %d\n", (dataLength * 8));
    printBits(data, dataLength);
    printf("===============\n");

    free(NALUnit);
    return sps;

    // TEMP FREEING
    if (sps->offsetForRefFrame != NULL)
        free(sps->offsetForRefFrame);

    free(sps);
}



void sliceHeaderDecode(u32 NALUnitDataLength, u8 *NALDataStream, sampleInfo *sample, MPEG_Data *videoData) { 
    NALUnitInfo *NALUnit = parseNALUnit(NALUnitDataLength, NALDataStream);

    u32 bitsRead = 0;
    u32 bytesRead = 0;

    u8 *data        = NALUnit->NALUnitData;
    u32 dataLength  = NALUnit->NALUnitDataLength;
    u8  NALUnitType = NALUnit->NALUnitType;

    printf("===============\n");
    sliceHeader *sh = calloc(1, sizeof(sliceHeader));

    sh->firstMbInSlice      = ue(data, &bitsRead, &bytesRead, dataLength);
    sh->sliceType           = ue(data, &bitsRead, &bytesRead, dataLength);
    printf("slice type: %d\n", sh->sliceType);
    sh->picParameterSetId   = ue(data, &bitsRead, &bytesRead, dataLength);
    
    u32 ppsTableIndex = linearSearch(sh->picParameterSetId, 
                                    videoData->avcData->picParamTable->picParameterSetIdArr,
                                    videoData->avcData->picParamTable->numberOfEntries,
                                    compu32);
    picParameterSet *pps = videoData->avcData->picParamTable->picParameterSetArr[ppsTableIndex];

    u32 spsTableIndex = linearSearch(pps->seqParameterSetId, 
                                    videoData->avcData->seqParamTable->seqParameterSetIdArr,
                                    videoData->avcData->seqParamTable->numberOfEntries,
                                    compu32);
    seqParameterSet *sps = videoData->avcData->seqParamTable->seqParameterSetArr[spsTableIndex];

    if (NALUnitType != 5) {
        // sps is reffered to in pps is meant to be equal to last decoding order slice pps' sps val
        // TD check if this holds true 
    } else { 
        // unknown special case, check iso
    }


    sh->frameNum            = u(data, &bitsRead, &bytesRead, sps->log2MaxFrameNumMinus4 + 4); // TD more logic present in iso
    if (!(sps->frameMbsOnlyFlag)) { 
        sh->fieldPicFlag        = u(data, &bitsRead, &bytesRead, 1);
        if (sh->fieldPicFlag)
            sh->bottomFieldFlag = u(data, &bitsRead, &bytesRead, 1);
    }

    if (NALUnitType == 5) { 
        sh->idrPicId    = ue(data, &bitsRead, &bytesRead, dataLength);
    }

    if (sps->picOrderCntType == 0) { 
        sh->picOrderCntLsb  = u(data, &bitsRead, &bytesRead, sps->log2MaxPicOrderCntLsbMinus4 + 4);
        if (pps->picOrderPresentFlag == 1 && !(sh->fieldPicFlag))
            sh->deltaPicOrderCntBottom = se(data, &bitsRead, &bytesRead, dataLength);
    }

    if (sps->picOrderCntType == 1 && !(sps->deltaPicOrderAlwaysZeroFlag)) { 
        sh->deltaPicOrderCnt        = calloc(2, sizeof(i32));

        sh->deltaPicOrderCnt[0]     = se(data, &bitsRead, &bytesRead, dataLength);
        if (pps->picOrderPresentFlag == 1 && !(sh->fieldPicFlag))
            sh->deltaPicOrderCnt[1] = se(data, &bitsRead, &bytesRead, dataLength);
    }

    if (pps->redundantPicCntPresentFlag) { 
        sh->redundantPicCnt = ue(data, &bitsRead, &bytesRead, dataLength);
    }

    if (sh->sliceType == 1 || sh->sliceType == 6) { // if sh->sliceType == B
        sh->directSpatialMvPredFlag = u(data, &bitsRead, &bytesRead, 1);
    }

    if (sh->sliceType == 0 || sh->sliceType == 5 || // sh->sliceType == P
        sh->sliceType == 3 || sh->sliceType == 8 || // sh->sliceType == SP
        sh->sliceType == 1 || sh->sliceType == 6) { // sh->sliceType == B
        
        sh->numRefIdxActiveOverrideFlag = u(data, &bitsRead, &bytesRead, 1);
        
        if (sh->numRefIdxActiveOverrideFlag) {
            sh->numRefIdx10ActiveMinus1 = ue(data, &bitsRead, &bytesRead, dataLength);
            if (sh->sliceType == 1 || sh->sliceType == 6) // if sh->sliceType == B
                sh->numRefIdx11ActiveMinus1 = ue(data, &bitsRead, &bytesRead, dataLength);
        }
    }

    //refPicListReordering();



    free(NALUnit);

}

void refPicListReordering() { 

}


u8 byteAligned(u32 *bitsRead) { 
    if ((*bitsRead % 8) == 7)
        return TRUE;

    return FALSE;
}

void rbspTrailingBits(u8 *data, u32 *bitsRead, u32 *bytesRead, u32 numberOfBits) {
    u8 rbspStopOneBit           = getUnsignedNBits(data, bitsRead, bytesRead, 1);
    printf("RBSP STOP ONE BIT: %d\n", rbspStopOneBit);

    u32 debugTrailingBitCounter = 1;
    while (byteAligned(bitsRead) == FALSE) { 
        u8 rbspAlignmentZeroBit = getUnsignedNBits(data, bitsRead, bytesRead, 1);
        debugTrailingBitCounter++;
    }

    printf("TOTAL TRAILING BITS: %d\n", debugTrailingBitCounter);
}
