
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>
#include "main.h"
#include "typesMPEG-4.h"

/**
 * @brief Holds a different type in currentItem and Node (same) type in nextNode
 * @param *currentItem  -   usually type of box
 * @param *nextNode     -   another node instance
 */
typedef struct Node { 
    void *currentItem;
    void *nextNode;
} Node;

/**
 * @brief Standard linked list implementation
 * @param *size     -   items in linkedlist
 * @param *head     -   first node
 * @param *tail     -   last node
 * @param *current  -   used for traversal to mimic "yeild"
 *
 * @note interface defined in linkedList.c/h
 */
typedef struct linkedList {
    u32 size;
    Node *head;
    Node *tail;
    Node *last;
    Node *current;
} linkedList;

linkedList* linkedlistInit();
void linkedlistAppendNode(linkedList *list, void *item);
void linkedlistNullifyLastNode(linkedList *list);

void linkedlistPrintAllBoxes(linkedList *list);

u32 linkedlistGetOffsetToBox(linkedList *list, u8 boxCompareType[]);
box *linkedlistGetBox(linkedList *list, u8 boxReturnType[]);

void freeBox(box *boxStruct);
Node *freeBoxNode(Node *nodeStruct);
void freeLinkedList(linkedList *list, char type[]);
void freeBoxLinkedListExclude(linkedList *list, char excludedType[]);

#endif // LINKED_LIST_H
