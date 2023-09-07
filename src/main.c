
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "main.h"

#include "typesMPEG-4.h"
#include "typesAVC.h"
#include "typesUtility.h"
#include "typesStorage.h"
#include "linkedlist.h"

#include "bitUtility.h"
#include "endianUtility.h"
#include "printUtility.h"
#include "memoryManagement.h"

#include "MPEG-4Parse.h"
#include "MPEG-4Decode.h"
#include "MPEG-4Process.h"
#include "AVCDecode.h"
#include "AVCMath.h"

int main(int argc, char **argv) { 
    MPEG_Data *videoData = getMpegData("local_files/op.mp4");

    createDisplayTimeToSampleTable(videoData);    
    getVideoDataRangeByMediaTime(15, 20, videoData);

    //sampleSearchByRealTime(15, videoData);
    //keyFrameSearch(79, videoData);

    // free every linked list created
    freeMpegData(videoData);
    
    printf("end of script\n");
    exit(0);
}