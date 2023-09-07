#ifndef COMMON_HEAD
    #define COMMON_HEAD
    #include <stdlib.h>
    #include <stdint.h>
    #include <string.h>
    #include <stdbool.h>
    #include <stdio.h>
    #include <math.h>
    #include <time.h>
    #include "main.h"
#endif //COMMON_HEAD

#ifndef COMMON_TYPES
    #define COMMON_TYPES
    #include "typesMPEG-4.h"
    #include "typesAVC.h"
    #include "typesUtility.h"
#endif //COMMON_TYPES

#ifndef DATA_STRUCTURES
    #define DATA_STRUCTURES
    #include "linkedlist.h"
    #include "typesStorage.h"
#endif //DATA_STRUCTURES

#ifndef COMMON_UTIL
    #define COMMON_UTIL
    #include "bitUtility.h"
    #include "endianUtility.h"
    #include "printUtility.h"
    #include "memoryManagement.h"
#endif //COMMON_UTIL

#ifndef MPEG_HEAD
    #define MPEG_HEAD
    #include "MPEG-4Parse.h"
    #include "MPEG-4Decode.h"
    #include "MPEG-4Process.h"
    #include "AVCDecode.h"
    #include "AVCMath.h"
#endif //MPEG_HEAD

int main(int argc, char **argv) { 
    // get path and video name from user

    // Use parseMPEG-4 interface to get video frame information
    // "pipe" that information into a the gif interface functional style

    MPEG_Data *videoData = getMpegData("local_files/op.mp4");
    createDisplayTimeToSampleTable(videoData);
    
    float startTime = (float)clock()/CLOCKS_PER_SEC;
    
    getVideoDataRangeByMediaTime(15, 20, videoData);
    
    float endTime = (float)clock()/CLOCKS_PER_SEC;
    float timeElapsed = endTime - startTime;
    printf("DATA PROCESSING OPERATION elapsed: %f\n", timeElapsed);
    
    
    //sampleSearchByRealTime(15, videoData);
    //keyFrameSearch(79, videoData);

    // free every linked list created
    freeMpegData(videoData);
    printf("end of script\n");
    return 0;
}