#include <stdio.h>                                                                                                                                                      

#include "../../include/Interface.h"

int main(int argc, char* argv[])
{
    RecordHandle handle =  RECORD_Open("flv|file://a.flv");
    RECORD_SetAudioInfo(handle,44100,2,16,1000,NULL,0);
    RECORD_SetVideoInfo(handle,480,360,25,1000,NULL,0);
    unsigned char spspps[] = {0x00,0x00,0x00,0x01,0x67,0xe1,0x00,0x00,0x00,0x01,0x68,0x35,0x00,0x00,0x00,0x01,0x65};
    unsigned char first[] = {0x00,0x00,0x00,0x01,0x65,0x01,0x01};

    RECORD_Frame frame;
    frame.buffer = spspps;
    frame.buffer_length = sizeof(spspps);
    frame.start_time = 0;
    frame.cts_delta = 0;
    RECORD_RecordPush(handle,rpt_video,&frame);

    frame.buffer = first;
    frame.buffer_length = sizeof(first);
    RECORD_RecordPush(handle,rpt_video,&frame);
    RECORD_RecordDestory(handle);
    return 0;
}

