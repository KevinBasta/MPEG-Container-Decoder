
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdint.h>
#include "main.h"

#include "linkedlist.h"
#include "typesMPEG-4.h"
#include "bitUtility.h"
#include "printUtility.h"

/**
 * @brief create a new linkedList
 * @return a new linked list struct with all values initialized
 */
linkedList* linkedlistInit() { 
    linkedList *newLinkedList = (linkedList*) calloc(1, sizeof(linkedList));
    
    newLinkedList->size = 0;

    newLinkedList->head = (Node*) calloc(1, sizeof(Node));
    newLinkedList->last = newLinkedList->head;
    newLinkedList->tail = newLinkedList->head;
    newLinkedList->current = newLinkedList->head;

    return newLinkedList;
}


/**
 * @brief place an item in the current node and create a new node
 * @param *list     -   the linkedList to append to
 * @param *item     -   the item to add to the tail node's currentItem
 */
void linkedlistAppendNode(linkedList *list, void *item) {
    list->tail->currentItem = item;
    list->last = list->tail;
    list->tail->nextNode = (Node*) calloc(1, sizeof(Node)); 

    list->tail = list->tail->nextNode;
    list->size += 1;
}

/**
 * @brief set last node's nextNode to null
 * @param *list     -   the linkedList affected  
 */
void linkedlistNullifyLastNode(linkedList *list) {
    //free(list->tail->nextNode); NextNode not initalized
    list->tail->nextNode = NULL;
}


/**
 * @brief print boxType & boxSize of each box in a linkedList
 * @param *list  -   the linkedList read from
 */
void linkedlistPrintAllBoxes(linkedList *list) { 
    if (DEBUG_PRINT_ENABLE == FALSE) { 
        return;
    }

    Node *currentNode = list->head;
    for (u32 i = 0; i < list->size; i++) { 
        box *currentBoxPointer = (box*) currentNode->currentItem;
        printNBytes(currentBoxPointer->boxType, 4, "box type: ", "\t");
        printf("box size: %10u\n", currentBoxPointer->boxSize);
        currentNode = currentNode->nextNode;
    }
}


/**
 * @brief accumulates all box sizes until finds boxCompareType
 * @param list              -   the linkedList to search in
 * @param boxCompareType    -   box to stop accumulating at
 * @return the sum of the sizes of all boxes preceding boxCompareType
 */
u32 linkedlistGetOffsetToBox(linkedList *list, u8 boxCompareType[]) { 
    u32 offsetAccumulator = 0;
    
    Node *currentNode = list->head;
    for (u32 i = 0; i < list->size; i++) { 
        box *currentBoxPointer = (box*) currentNode->currentItem;

        if ((compareNBytes(currentBoxPointer->boxType, boxCompareType, 4) == TRUE)) {
            return offsetAccumulator;
        } else { 
            offsetAccumulator += currentBoxPointer->boxSize;
        }

        currentNode = currentNode->nextNode;
    }

    return offsetAccumulator;
}


/**
 * @brief find and return box by its boxType
 * @param *list             -   the linkedList searched
 * @param *boxReturnType    -   the type to find    
 * @return NULL or box pointer to a box of type *boxReturnType
 */
box *linkedlistGetBox(linkedList *list, u8 boxReturnType[]) { 
    box *boxToReturn = NULL;
    list->current = list->head;

    for (u32 i = 0; i < list->size; i++) {
        box *currentBoxPointer = (box*) list->current->currentItem;

        if ((compareNBytes(currentBoxPointer->boxType, boxReturnType, 4) == TRUE)) {
            // DEBUG printf("True, returning\n");
            boxToReturn = list->current->currentItem;

            list->current = list->current->nextNode;

            return boxToReturn;
        }

        list->current = list->current->nextNode;
    }

    if (boxToReturn == NULL) { 
        printf("No (Other) Match Found For: %s\n", boxReturnType);
    }

    return boxToReturn;
}


/**
 * @brief reset list->current to allow for another linkedlistGetBox
 * from the start of the linkedList
 * @param list      -   the linkedlist to reset list->current for
 */
void linkedlistResetCurrentNode(linkedList *list) { 
    list->current = list->head;
}


/**
 * @brief free ALL fields of a box. all fields must be allocations 
 * and not references
 * @param boxStruct     -   the struct to free the fields of
 */
void freeBox(box *boxStruct) { 
    // this is only required for boxes who's 
    // fields are allocations and not references
    // to their parent container
    free(boxStruct->boxType);
    free(boxStruct->boxData);

    // freeing the box itself since it holds pointer values
    free(boxStruct);
}


/**
 * @brief call a function to free the currentItem
 * @param nodeStruct    -   node to free
 * @return  the next node
 *
 * @note can generalize as needed 
 */
Node *freeBoxNode(Node *nodeStruct) { 
    Node *nextNode = nodeStruct->nextNode;

    freeBox(nodeStruct->currentItem);
    // Note: free(nodeStruct->currentItem) handeled by freeBox
    // Note: not freeing nodeStruct->nextNode handeled by freeLinkedLists if last
    free(nodeStruct);

    return nextNode;
}

void freeBoxLinkedListExclude(linkedList *list, char excludedType[]) {
    Node *currentNode = list->head;

    // stopping right before last element to avoid 
    // attempting to free a NULL created by linkedlistNullifyLastNode


    for (u32 i = 0; i < list->size - 1; i++) {
            if (strncmp(excludedType, ((box*) currentNode->currentItem)->boxType, strlen(excludedType)) != 0) {
                currentNode = freeBoxNode(currentNode);
            } else {
                currentNode = currentNode->nextNode;
            }
            // DEBUG printf("%d %d\n", currentNode, currentNode->nextNode);
            list->head = currentNode;
    }

    // freeing the last node
    if (strncmp(excludedType, ((box*) currentNode->currentItem)->boxType, strlen(excludedType)) != 0) {
        freeBox(currentNode->currentItem);
    }

    free(currentNode);
    free(list);
}

/**
 * @brief free all nodes in a linkedList
 * @param list  -   list to free
 * @param type  -   type of items contained in linkedList
 */
void freeLinkedList(linkedList *list, char type[]) { 

    Node *currentNode = list->head;

    // stopping right before last element to avoid 
    // attempting to free a NULL created by linkedlistNullifyLastNode
    if (strncmp(type, "box", 3)) { 
        for (u32 i = 0; i < list->size - 1; i++) {
                currentNode = freeBoxNode(currentNode);
                // DEBUG printf("%d %d\n", currentNode, currentNode->nextNode);
                list->head = currentNode;
        }

        // freeing the last node
        freeBox(currentNode->currentItem);
        if (currentNode->nextNode != NULL) { 
            // this case should not happen
            printf("error occured in freeLinkedList final node\n");
            free(currentNode->nextNode);
        }
        free(currentNode);
    }

    free(list);
}

/* int main() {
    linkedList *test = linkedlistInit();
    box *testBox1 = (box*) malloc(sizeof(box));
    box *testBox2 = (box*) malloc(sizeof(box));
    box *testBox3 = (box*) malloc(sizeof(box));
    linkedlistAppendNode(test, testBox1);
    linkedlistAppendNode(test, testBox2);
    linkedlistAppendNode(test, testBox3);

    freeLinkedList(test, "box");
} */