#include "FlvVideoTransfer.h"

namespace record
{
    FlvVideoTransfer::FlvVideoTransfer(void)
    {
    }

    FlvVideoTransfer::~FlvVideoTransfer(void)
    {
    }

    void FlvVideoTransfer::transfer(Sample & sample)
    {
        VideoTag videoHeader;
        videoHeader.date_size = sample.size+5;
        videoHeader.timestample = sample.time;
        if (sample.flags == Sample::sync)
            videoHeader.FrameType = 1;
        else
            videoHeader.FrameType = 2;

        videoHeader.CompositionTime = sample.cts_delta; 
        videoHeader.OArchive();

        sample.data.push_front(const_buffer((uint8_t *)&videoHeader,sizeof(VideoTag),true));
        sample.size += sizeof(VideoTag);

        uint32_t pre_size = sample.size;
        local_to_bigendian((uint8_t *)&pre_size,sizeof(uint32_t)); 

        sample.data.push_back(const_buffer((uint8_t *)&pre_size,sizeof(uint32_t),true));
        sample.size += sizeof(uint32_t);
    }
}
