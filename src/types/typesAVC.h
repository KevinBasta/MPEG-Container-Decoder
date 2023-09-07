
#ifndef AVC_TYPES_H
#define AVC_TYPES_H

#include <stdint.h>
#include "main.h"

// AVC Structs //
typedef struct NALUnitInfo {
    u8 NALRefIdc;
    u8 NALUnitType;
    u32 NALUnitDataLength;
    u8 *NALUnitData;
} NALUnitInfo;

typedef struct picParameterSet {
    u32 picParameterSetId;
    u32 seqParameterSetId;
    u8 entropyCodingModeFlag;
    u8 picOrderPresentFlag;
    u32 numSliceGroupsMinus1;

    u32 sliceGroupMapType;
    u32 *runLengthMinus1;
    u32 *topLeft;
    u32 *bottomRight;
    u8 sliceGroupChangeDirectionFlag;
    u32 sliceGroupChangeRateMinus1;
    u32 picSizeInMapUnitsMinus1;
    u32 *sliceGroupId;

    u32 numRefIdxl0ActiveMinus1;
    u32 numRefIdxl1ActiveMinus1;
    u8 weightedPredFlag;
    u8 weightedBipredIdc;
    i32 picInitQpMinus26;
    i32 picInitQsMinus26;
    i32 chromaQpIndexOffset;
    u8 deblockingFilterVariablesPresentFlag;
    u8 constrainedIntraPredFlag;
    u8 redundantPicCntPresentFlag;
    u8 frameCroppingFlag;

    u32 frameCropLeftOffset;
    u32 frameCropRightOffset;
    u32 frameCropTopOffset;
    u32 frameCropBottomOffset;
} picParameterSet;


typedef struct seqParameterSet { 
    u8 profileIdc;
    u8 levelIdc;
    u8 moreThanOneSliceGroupAllowedFlag;
    u8 arbitrarySliceOrderAllowedFlag;
    u8 redundantPicturesAllowedFlag;
    u32 seqParameterSetId;
    u32 log2MaxFrameNumMinus4;
    u32 picOrderCntType;

    u32 log2MaxPicOrderCntLsbMinus4;
    u8 deltaPicOrderAlwaysZeroFlag;
    i32 offsetForNonRefPic;
    i32 offsetForTopToBottomField;
    u32 numRefFramesInPicOrderCntCycle;
    i32 *offsetForRefFrame;

    u32 numRefFrames;
    u8 requiredFrameNumUpdateBehaviourFlag;
    u32 picWidthInMbsMinus1;
    u32 picHeightInMapUnitsMinus1;
    u8 frameMbsOnlyFlag;

    u8 mbAdaptiveFrameFieldFlag;
    u8 direct8x8InferenceFlag;
    u8 vuiParametersPresentFlag;
} seqParameterSet;


/**
 * @brief picParameterSetId holds id corrosponding to each
 * picParameterSets entry for searching and sorting
 */
typedef struct picParameterSetTable {
    u32 numberOfEntries;
    picParameterSet **picParameterSetArr;
    u32 *picParameterSetIdArr;
} picParameterSetTable;


/**
 * @brief seqParameterSetId holds id corrosponding to each
 * seqParameterSets entry for searching and sorting
 */
typedef struct seqParameterSetTable { 
    u32 numberOfEntries;
    seqParameterSet **seqParameterSetArr;
    u32 *seqParameterSetIdArr;
} seqParameterSetTable;


typedef struct sliceHeader { 
    u32 firstMbInSlice;
    u32 sliceType;
    u32 picParameterSetId;
    u32 frameNum;
    u8 fieldPicFlag;
    u8 bottomFieldFlag;
    u32 idrPicId;
    u32 picOrderCntLsb;
    i32 deltaPicOrderCntBottom;
    i32 *deltaPicOrderCnt;
    u32 redundantPicCnt;
    u8 directSpatialMvPredFlag;
    u8 numRefIdxActiveOverrideFlag;
    u32 numRefIdx10ActiveMinus1;
    u32 numRefIdx11ActiveMinus1;
    u32 cabacInitIdc;
    i32 sliceQpDelta;
    u8 spForSwitchFlag;
    i32 sliceQsDelta;
    u32 disableDeblockingFilterIdc;
    i32 sliceAlphaC0OffsetDiv2;
    i32 sliceBetaOffsetDiv2;
    u32 sliceGroupChangeCycle;
} sliceHeader;

typedef struct AVC_Data {
    // avcc length of nal unit
    u8 lengthSizeMinus1;
    picParameterSetTable *picParamTable;
    seqParameterSetTable *seqParamTable;
} AVC_Data;

typedef struct NAL_Data {
    u32 placeholder;
} NAL_Data;


typedef struct parsingInfo { 
    u32 *bitsRead;
    u32 *bytesRead;
    u8 *data;
} parsingInfo;

#endif // AVC_TYPES_H