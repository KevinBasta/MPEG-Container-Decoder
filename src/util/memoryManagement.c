
#include <stdlib.h>

#include <stdint.h>
#include "main.h"

#include "typesMPEG-4.h"
#include "typesAVC.h"
#include "linkedlist.h"
#include "typesStorage.h"

u8 isNULL(void *data) {
    if (data == NULL) { 
        return TRUE;
    } else {
        return FALSE;
    }
}

void safeFree(void *data) { 
    if (data != NULL) {
        free(data);
    }
}

/////////// MPEG_Data Fields ///////////
void freeTopBoxesLL(linkedList *data) { 
    if (isNULL(data))
        return;

    freeLinkedList(data, "box");
}

void freeElstTable(elstTable *data) { 
    if (isNULL(data))
        return;

    safeFree(data->trackDurationArr);
    safeFree(data->mediaTimeArr);
    safeFree(data->mediaRateArr);
    safeFree(data);
}

void freeDataRefTable(dataReferenceTableEntry **data) { 
    if (isNULL(data))
        return;

    u32 i = 0;
    while (data[i] != NULL) { 
        free(data[i]->type);
        free(data[i]->version);
        free(data[i]->flags);
        free(data[i]->data);
        free(data[i]);
        i++;
    }

    free(data);
}

void freeTimeToSampleTableCompressed(timeToSampleTableCompressed *data) { 
    if (isNULL(data))
        return;

    safeFree(data->sampleCountArr);
    safeFree(data->sampleDurationArr);
    safeFree(data);
}

void freeTimeToSampleTable(timeToSampleTable *data) { 
    if (isNULL(data))
        return;

    safeFree(data->sampleDurationArr);
    safeFree(data->sampleDeltaArr);
    safeFree(data);
}

void freeSampleToChunkTable(sampleToChunkTable *data) { 
    if (isNULL(data))
        return;

    safeFree(data->firstChunkArr);
    safeFree(data->samplesPerChunkArr);
    safeFree(data->sampleDescriptionIdArr);
    safeFree(data);
}

void freeChunkOffsetTable(chunkOffsetTable *data) {
    if (isNULL(data))
        return;

    safeFree(data->offsetArr);
    safeFree(data);
}

void freeSampleSizeTable(sampleSizeTable *data) { 
    if (isNULL(data))
        return;

    safeFree(data->sizeArr);
    safeFree(data);
}

void freeSyncSampleTable(syncSampleTable *data) { 
    if (isNULL(data))
        return;

    safeFree(data->sampleNumberArr);
    safeFree(data);
}

void freeCompositionOffsetTableCompressed(compositionOffsetTableCompressed *data) { 
    if (isNULL(data))
        return;

    safeFree(data->sampleCountArr);
    safeFree(data->compositionOffsetArr);
    safeFree(data);
}

void freeCompositionOffsetTable(compositionOffsetTable *data) { 
    if (isNULL(data))
        return;

    safeFree(data->compositionOffsetArr);
    safeFree(data);
}

void freeDisplayTimeToSampleTable(displayTimeToSampleTable *data) {
    if (isNULL(data))
        return;

    safeFree(data->displayTimeArr);
    safeFree(data->sampleNumberArr);
    safeFree(data);
}

void freeMpegData(MPEG_Data *videoData) {
    freeTopBoxesLL                          (videoData->topBoxesLL);
    freeElstTable                           (videoData->elstTable);
    freeDataRefTable                        (videoData->dataReferenceTable);
    freeTimeToSampleTableCompressed         (videoData->timeToSampleTableCompressed);
    freeTimeToSampleTable                   (videoData->timeToSampleTable);
    freeSampleToChunkTable                  (videoData->sampleToChunkTable);
    freeChunkOffsetTable                    (videoData->chunkOffsetTable);
    freeSampleSizeTable                     (videoData->sampleSizeTable);
    freeSyncSampleTable                     (videoData->syncSampleTable);
    freeCompositionOffsetTableCompressed    (videoData->compositionOffsetTableCompressed);
    freeCompositionOffsetTable              (videoData->compositionOffsetTable);
    freeDisplayTimeToSampleTable            (videoData->displayTimeToSampleTable);

    free(videoData);
}




// for tables with no pointer values
// cast param passed using (void**)
/* void freeTable(void **table) {
    u32 i = 0;
    while (table[i] != NULL) { 
        free(table[i]);
        i++;
    }
    free(table);
} */

void freeSampleDescription() { 

}