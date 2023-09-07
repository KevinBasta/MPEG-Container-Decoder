
#ifndef MPEG_DECODE_H
#define MPEG_DECODE_H

#include <stdint.h>
#include "main.h"
#include "typesStorage.h"
#include "typesMPEG-4.h"

void ftypParseBox(box *ftypBox);
void mvhdParseBox(box *mvhdBox, MPEG_Data *videoData);
void tkhdParseBox(box *trakBox, MPEG_Data *videoData);
void elstParseBox(box *elstBox, MPEG_Data *videoData);
void edtsParseBox(box *edtsBox, MPEG_Data *videoData);
void mdhdParseBox(box *mdhdBox, MPEG_Data *videoData);
u8 *hdlrParseBox(box *hdlrBox);
void vmhdParseBox(box *vmhdBox);
void drefParseBox(box *drefBox, MPEG_Data *videoData);
void dinfParseBox(box *dinfBox, MPEG_Data *videoData);

void stsdParseBox(box *stsdBox, MPEG_Data *videoData);
void stszParseBox(box *stszBox, MPEG_Data *videoData);
void stscParseBox(box *stscBox, MPEG_Data *videoData);
void stcoParseBox(box *stcoBox, MPEG_Data *videoData);
void sttsParseBox(box *sttsBox, MPEG_Data *videoData);
void stssParseBox(box *stssBox, MPEG_Data *videoData);
void cttsParseBox(box *cttsBox, MPEG_Data *videoData);

void paspParseBox(box *paspBox, MPEG_Data *videoData);
void btrtParseBox(box *btrtBox, MPEG_Data *videoData);
void colrParseBox(box *colrBox, MPEG_Data *videoData);
void avccParseBox(box *avccBox, MPEG_Data *videoData);

#endif // MPEG_DECODE_H
