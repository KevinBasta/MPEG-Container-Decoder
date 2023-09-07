
#include <stdint.h>
#include "main.h"

// search algorithm for stss/sync sample
// returns the preceeding 
u32 binarySearch(u32 target, u32 *table, u32 totalEntries, i32 (*comp) (const void *, const void *)) { 
    u32 low = 0;
    u32 high = totalEntries;
    u32 mid; 
    
    while (low <= high) { 
        mid = (low + high) / 2;
        i32 result = comp(&target, &table[mid]); 

        if (result == 0) { 
            return mid;
        } else if (result <  0) { 
            high = mid - 1;
        } else if (result > 0) { 
            low = mid + 1;
        }

        //printf("%d %d %d %d %d\n", low, high, mid, sampleNumber, syncSampleTable[mid]->number);
    }

    return low; // currently used as a fallback value in processMPEG
}


// used for slice header searching for it's pps id
// can be deprecated in future if pps is always in id order
// alternatively can sort and search, but **pps doesn't seem to have
// many entries
// returns index 
u32 linearSearch(u32 target, u32 *array, u32 totalEntries, i32 (*comp) (const void *, const void *)) { 
    
    for (int i = 0; i < totalEntries; i++) {
        i32 result = comp(&target, &array[i]);

        if (result == 0) { 
            return i;
        }
    }

    return 0; // error return? can make return type i64 and return -1 for error checking since index is returned
}


/* i32 compPpsId(const void *a, const void *b) {
    picParameterSet *c = (picParameterSet*) a;
    picParameterSet *d = (picParameterSet*) b;

    i32 arg1 = *(const u32*) c->picParameterSetId;
    i32 arg2 = *(const u32*) d->picParameterSetId;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
} */

/* i32 compSpsId() {} */

i32 compi32(const void *a, const void *b) { 
    i32 arg1 = *(const int*) a;
    i32 arg2 = *(const int*) b;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

i32 compu32(const void *a, const void *b) { 
    u32 arg1 = *(const int*) a;
    u32 arg2 = *(const int*) b;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}


// quick sort
void swap (u32 *arr1, u32 *arr2, u32 firstIndex, u32 secondIndex) { 
    u32 arr1Temp = arr1[firstIndex];
    u32 arr2Temp = arr2[firstIndex];

    arr1[firstIndex] = arr1[secondIndex];
    arr2[firstIndex] = arr2[secondIndex];

    arr1[secondIndex] = arr1Temp;
    arr2[secondIndex] = arr2Temp;
}

u32 partition(u32 *arr1, u32 *arr2, u32 low, u32 high) { 
    int i = (low - 1);

    int pivot = arr1[high];

    int j;
    for (j = low; j < high; j++) {
        if (arr1[j] <= pivot){
            i++;
            swap(arr1, arr2, i, j);
        }
    }
    swap(arr1, arr2, i + 1, high);
    return (i + 1);
}

void quickSort(u32 *arr1, u32 *arr2, u32 low, u32 high) { 
    if (low < high) { 
        u32 pivotIndex = partition(arr1, arr2, low, high);

        quickSort(arr1, arr2, low, pivotIndex - 1);
        quickSort(arr1, arr2, pivotIndex + 1, high);
    }
}


// bubble sort
void bubbleSort(u32 *arr1, u32* arr2, int n) {
    int swapped;
    for (int i = 0; i < n - 1; i++) {
        swapped = FALSE;
        for (int j = 0; j < n - i - 1; j++) {
            if (arr1[j] > arr1[j + 1]) {
                swap(arr1, arr2, j, j+1);
                swapped = TRUE;
            }
        }
 
        if (swapped == FALSE)
            break;
    }
}
