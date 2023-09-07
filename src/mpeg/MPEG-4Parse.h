
#ifndef MPEG_PARSE_H
#define MPEG_PARSE_H

#include <stdint.h>
#include "main.h"
#include "linkedlist.h"
#include "typesStorage.h"

MPEG_Data *getMpegData(char *fileName);
void readMainBoxes(u8 fileName[], linkedList *list);
void parseChildBoxes(box *parentBox, linkedList *list);
void parseNestedChildBoxes(u8 *boxData, u32 *bytesRead, u32 endIndex, linkedList *list);
box *parseSingleNestedChildBox(u8 *boxData, u32 *bytesRead);
box *getVideTrak(linkedList *moovLL);

#endif // MPEG_PARSE_H
